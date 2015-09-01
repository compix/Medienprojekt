#include "Graph.h"
#include "../LayerManager.h"
#include "../GameConstants.h"
#include "../Game.h"
#include "../Components/BlockComponent.h"
#include "../Components/SolidBlockComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/TimerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/PortalComponent.h"
#include "../Components/DestructionComponent.h"
#include <queue>
#include "../Components/LavaComponent.h"

Graph::Graph(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
	auto mainLayer = m_layerManager->getLayer(GameConstants::MAIN_LAYER);
	
	m_width = mainLayer->getWidth();
	m_height = mainLayer->getHeight();

	m_nodeGrid = new GraphNode*[m_width];

	for (int i = 0; i < m_width; i++)
	{
		m_nodeGrid[i] = new GraphNode[m_height];
	}

	mainLayer->listen(this);
	reset();
}

Graph::~Graph()
{
	for (int i = 0; i < m_width; i++)
	{
		delete[] m_nodeGrid[i];
	}
		
	delete[] m_nodeGrid;
}

void Graph::update(float deltaTime)
{
	m_normalBombs.clear();
	m_blocksAffectedByExplosion.clear();

	// Reset some dynamic properties
	for (auto x = 1; x < m_width - 1; ++x)
	{
		for (auto y = 1; y < m_height - 1; ++y)
		{
			m_nodeGrid[x][y].properties.affectedByExplosion = false;
			m_nodeGrid[x][y].properties.timeTillExplosion = 0.f;
			m_nodeGrid[x][y].smells.dyingBlock = 0;
			m_nodeGrid[x][y].smells.enemy = 0;
		}
	}

	// Link portals
	for (auto portal : GameGlobals::entities->entities_with_components<CellComponent, PortalComponent>())
	{	
		auto portalComponent = portal.component<PortalComponent>();
		if (portalComponent->otherPortal)
		{
			auto cell = portal.component<CellComponent>();
			auto otherCell = portalComponent->otherPortal.component<CellComponent>();
			m_nodeGrid[cell->x][cell->y].properties.otherPortal = &m_nodeGrid[otherCell->x][otherCell->y];
		}
	}

	for (auto lava : GameGlobals::entities->entities_with_components<LavaComponent, CellComponent>())
	{
		auto cell = lava.component<CellComponent>();
		m_nodeGrid[cell->x][cell->y].properties.affectedByExplosion = true;
		if (lava.has_component<TimerComponent>())
			m_nodeGrid[cell->x][cell->y].properties.timeTillExplosion = std::min(2.f, lava.component<TimerComponent>()->seconds);
		else
			m_nodeGrid[cell->x][cell->y].properties.timeTillExplosion = 0.f;
	}

	for (auto dyingBlock : GameGlobals::entities->entities_with_components<BlockComponent, DestructionComponent, CellComponent>())
	{
		if (!dyingBlock.valid())
			continue;

		auto cell = dyingBlock.component<CellComponent>();
		m_nodeGrid[cell->x][cell->y].properties.affectedByExplosion = true;
		m_blocksAffectedByExplosion.push_back(dyingBlock);
	}

	for (auto bombEntity : GameGlobals::entities->entities_with_components<BombComponent, TimerComponent, CellComponent>())
	{
		auto bombComponent = bombEntity.component<BombComponent>();
		auto timerComponent = bombEntity.component<TimerComponent>();
		auto cell = bombEntity.component<CellComponent>();

		//m_nodeGrid[cell->x][cell->y].properties.hasBomb = true;
		m_normalBombs.push_back(Bomb(cell->x, cell->y, bombComponent->explosionRange, timerComponent->seconds, bombComponent->type));
		auto& bombProperties = m_nodeGrid[cell->x][cell->y].bombProperties;
		bombProperties = BombProperties(bombComponent->explosionRange, timerComponent->seconds, bombComponent->type);
	}

	// Go through all explosion components and simulate the explosion.
	for (auto explosion : GameGlobals::entities->entities_with_components<ExplosionComponent, SpreadComponent, CellComponent>())
	{
		auto cell = explosion.component<CellComponent>();
		auto spread = explosion.component<SpreadComponent>();

		float exploTime = std::max(0.f, spread->timeTillNext);
		setOnFire(cell->x, cell->y, exploTime);

		if (spread->stopped)
			continue;

		ExplosionSpread eSpread(cell->x, cell->y, spread->range, exploTime, spread->direction, spread->bombType);
		explosionSpread(eSpread);
	}

	// Sort by explosionTime to simulate the correct explosion chain
	std::sort(m_normalBombs.begin(), m_normalBombs.end(), [](const Bomb& b1, const Bomb& b2) {return b1.explosionTime < b2.explosionTime; });

	for (auto& bomb : m_normalBombs)
	{
		if (!m_nodeGrid[bomb.x][bomb.y].bombProperties.explosionSimulated)
			placeBomb(bomb);
	}

	for (auto& dyingBlock : m_blocksAffectedByExplosion)
	{
		if (!dyingBlock)
			continue;

		auto cell = dyingBlock.component<CellComponent>();
		assert(cell);
		spreadSmell(SmellType::DYING_BLOCK, cell->x, cell->y, 5);
	}
}

void Graph::explosionSpread(const ExplosionSpread& spread, AffectedByExplosion* affectedEntities)
{
	float explosionTime = spread.explosionTime;
	auto currentNode = getNode(spread.x, spread.y);

	// Simulate the explosion:
	for (int j = 0; j < spread.range; ++j)
	{
		auto neighbor = getNeighbor(currentNode, spread.direction);

		if (!neighbor || neighbor->properties.hasSolidBlock)
			break;

		currentNode = neighbor;
		explosionTime += GameConstants::EXPLOSION_SPREAD_TIME;

		if (currentNode->properties.hasPlayer && affectedEntities)
		{
			std::vector<entityx::Entity> players = m_layerManager->getEntitiesWithComponents<InventoryComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y);
			affectedEntities->players.insert(affectedEntities->players.end(), players.begin(), players.end());
		}

		if (currentNode->valid)
		{			
			setOnFire(currentNode->x, currentNode->y, explosionTime);
			if (currentNode->properties.hasItem)
			{
				if (affectedEntities)
					affectedEntities->numOfItems++;

				if (spread.bombType == BombType::GHOST)
					continue; 

				break; // Items stop explosions.
			}
		}
		else
		{
			if (currentNode->properties.hasBlock)
			{
				// A block already affected by an explosion could potentially have an item. Another bomb could destroy this item
				if (currentNode->properties.affectedByExplosion && affectedEntities)
				{
					if (std::abs(currentNode->properties.timeTillExplosion - explosionTime) >= 0.25f)
						affectedEntities->numOfItems++;

					if (currentNode->properties.timeTillExplosion <= explosionTime)
					{
						if (spread.bombType == BombType::GHOST)
							continue;

						break;
					}
				}

				setOnFire(currentNode->x, currentNode->y, explosionTime);

				if (affectedEntities)
					affectedEntities->numOfBlocks++;

				auto block = m_layerManager->getEntityWithComponent<BlockComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y);
				if (block.valid())
					m_blocksAffectedByExplosion.push_back(block);
			}

			if (currentNode->properties.hasBomb)
			{
				setOnFire(currentNode->x, currentNode->y, explosionTime);
				if (!currentNode->bombProperties.explosionSimulated)
				{
					// Simulate the explosion chain
					auto& bombProperties = currentNode->bombProperties;
					Bomb bomb(currentNode->x, currentNode->y, bombProperties.explosionRange, explosionTime, bombProperties.type);
					placeBomb(bomb);
				}

				continue; // Bombs don't stop explosions
			}

			if (spread.bombType == BombType::GHOST && !currentNode->properties.hasSolidBlock)
				continue;

			break; // Explosion was stopped so get outta here.
		}
	}

	if (spread.bombType == BombType::LIGHTNING && currentNode && (spread.x != currentNode->x || spread.y != currentNode->y))
	{
		Bomb bomb(currentNode->x, currentNode->y, 1, explosionTime, BombType::NORMAL);
		placeBomb(bomb, affectedEntities);
	}
}

void Graph::setOnFire(uint8_t x, uint8_t y, float explosionTime)
{
	auto& node = m_nodeGrid[x][y];
	float currentExplosionTime = node.properties.timeTillExplosion;
	node.properties.timeTillExplosion = node.properties.affectedByExplosion ? std::min(explosionTime, currentExplosionTime) : explosionTime;
	node.properties.affectedByExplosion = true;

	// Duplicate the explosion on connected portals
	if (node.properties.otherPortal)
	{
		node.properties.otherPortal->properties.timeTillExplosion = node.properties.timeTillExplosion;
		node.properties.otherPortal->properties.affectedByExplosion = true;
	}
}

void Graph::spreadSmell(SmellType smellType, uint8_t startX, uint8_t startY, uint8_t range)
{
	// A simple breadth first search algorithm
	assert(inBounds(startX, startY));

	std::queue<GraphNode*> processQueue;
	GraphNode& start = m_nodeGrid[startX][startY];
	processQueue.push(&start);
	// Highest smell at the start -> then decreases by 1 for each cell
	start.smell(smellType) = std::max(start.smell(smellType), uint16_t(range));

	while (processQueue.size() > 0)
	{
		GraphNode* curNode = processQueue.front();
		processQueue.pop();

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighbor = getNeighbor(curNode, static_cast<Direction>(i));

			if (!neighbor || (!neighbor->properties.hasBomb && !neighbor->valid))
				continue;
			
			// If there is already a higher smell value of the given type on the cell then skip this node
			if (neighbor->smell(smellType) < curNode->smell(smellType))
			{
				processQueue.push(neighbor);
				neighbor->smell(smellType) = curNode->smell(smellType) - 1;
			}
		}
	}
}

void Graph::placeBomb(const Bomb& bomb, AffectedByExplosion* affectedEntities)
{	
	auto currentNode = getNode(bomb.x, bomb.y);
	currentNode->bombProperties.explosionRange = bomb.range;
	currentNode->bombProperties.explosionSimulated = true;

	float correctExplosionTime = currentNode->properties.affectedByExplosion ? std::min(bomb.explosionTime, currentNode->properties.timeTillExplosion) : bomb.explosionTime;
	setOnFire(bomb.x, bomb.y, correctExplosionTime);

	for (int i = 0; i < 4; ++i) // Go in all directions
	{
		Direction direction = static_cast<Direction>(static_cast<int>(Direction::UP) + i);
		ExplosionSpread explosion(bomb.x, bomb.y, bomb.range, correctExplosionTime, direction, bomb.type);
		explosionSpread(explosion, affectedEntities);
	}
}

void Graph::resetMarks()
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y].marked = false;
		}
	}
}

void Graph::spreadEnemySmells(entityx::Entity& self)
{
	resetEnemySmells();
	for (auto entity : GameGlobals::entities->entities_with_components<InventoryComponent, CellComponent>())
	{
		if (entity == self)
			continue;

		auto cell = entity.component<CellComponent>();
		spreadSmell(SmellType::ENEMY, cell->x, cell->y, 5);
	}
}

void Graph::resetEnemySmells()
{
	for (auto x = 1; x < m_width - 1; ++x)
	{
		for (auto y = 1; y < m_height - 1; ++y)
		{
			m_nodeGrid[x][y].smells.enemy = 0;
		}
	}
}

void Graph::reset()
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y] = GraphNode();
			m_nodeGrid[x][y].x = x;
			m_nodeGrid[x][y].y = y;
		}
	}
}

void Graph::addNode(uint8_t x, uint8_t y)
{
	assert(x < m_width && y < m_height);

	auto& node = m_nodeGrid[x][y];
	node.valid = true;
	node.x = x;
	node.y = y;
}

void Graph::removeNode(uint8_t x, uint8_t y)
{
	assert(x < m_width && y < m_height);
	m_nodeGrid[x][y].valid = false;
}

void Graph::onEntityAdded(entityx::Entity& entity)
{
	assert(entity.has_component<CellComponent>());
	auto cell = entity.component<CellComponent>();

	if (entity.has_component<InventoryComponent>())
	{
		m_nodeGrid[cell->x][cell->y].properties.hasPlayer = true;
		return;
	}

	if (entity.has_component<BombComponent>())
	{
		removeNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasBomb = true;
		return;
	}

	if (entity.has_component<ItemComponent>())
	{
		m_nodeGrid[cell->x][cell->y].properties.hasItem = true;
		return;
	}

	if (entity.has_component<PortalComponent>())
	{
		m_nodeGrid[cell->x][cell->y].properties.hasPortal = true;
		return;
	}

	if (entity.has_component<BlockComponent>())
	{
		removeNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasBlock = true;
		return;
	}

	if (entity.has_component<SolidBlockComponent>())
	{
		removeNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasSolidBlock = true;
	}	
}

void Graph::onEntityRemoved(entityx::Entity& entity)
{
	assert(entity.has_component<CellComponent>());
	auto cell = entity.component<CellComponent>();

	if (entity.has_component<InventoryComponent>())
	{
		// Multiple players on one cell are possible
		m_nodeGrid[cell->x][cell->y].properties.hasPlayer = m_layerManager->hasEntityWithComponent<InventoryComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y);
		return;
	}

	if (entity.has_component<BombComponent>())
	{
		// Bomb could be on a block or another bomb
		if (!m_layerManager->hasEntityWithOneComponent<BombComponent, BlockComponent, SolidBlockComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
			addNode(cell->x, cell->y);
		// Multiple bombs on one cell are possible
		m_nodeGrid[cell->x][cell->y].properties.hasBomb = m_layerManager->hasEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y);
		return;
	}
	
	if (entity.has_component<ItemComponent>())
	{
		if(!m_layerManager->hasEntityWithComponent<ItemComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
			m_nodeGrid[cell->x][cell->y].properties.hasItem = false;
		return;
	}

	if (entity.has_component<PortalComponent>())
	{
		// 2 portals on one cell? Shouldn't happen.
		assert(!m_layerManager->hasEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y));
		m_nodeGrid[cell->x][cell->y].properties.hasPortal = false;
		m_nodeGrid[cell->x][cell->y].properties.otherPortal = nullptr;
		return;
	}

	if (entity.has_component<BlockComponent>())
	{
		// 2 blocks on one cell? Shouldn't happen.
		assert(!m_layerManager->hasEntityWithComponent<BlockComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y));
		if (!m_layerManager->hasEntityWithOneComponent<BombComponent, SolidBlockComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
			addNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasBlock = false;
		return;
	}

	if (entity.has_component<SolidBlockComponent>())
	{
		// 2 solid blocks on one cell? Shouldn't happen.
		assert(!m_layerManager->hasEntityWithComponent<SolidBlockComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y));
		if (!m_layerManager->hasEntityWithOneComponent<BombComponent, BlockComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
			addNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasSolidBlock = false;
	}
}

GraphNode* Graph::getNeighbor(const GraphNode* node, const Direction& neighbor)
{
	GraphNode* portalNode;
	switch (neighbor)
	{
	case Direction::LEFT:
		if (node->x == 0)
			return nullptr;

		portalNode = getPortalNeighbor(node->x - 1, node->y);
		return portalNode ? portalNode : &m_nodeGrid[node->x - 1][node->y];
	case Direction::RIGHT:
		if (node->x == m_width - 1)
			return nullptr;

		portalNode = getPortalNeighbor(node->x + 1, node->y);
		return portalNode ? portalNode : &m_nodeGrid[node->x + 1][node->y];
	case Direction::UP:
		if (node->y == 0)
			return nullptr;

		portalNode = getPortalNeighbor(node->x, node->y - 1);
		return portalNode ? portalNode : &m_nodeGrid[node->x][node->y - 1];
	case Direction::DOWN:
		if (node->y == m_height - 1)
			return nullptr;

		portalNode = getPortalNeighbor(node->x, node->y + 1);
		return portalNode ? portalNode : &m_nodeGrid[node->x][node->y + 1];
	default: 
		assert(false); // Invalid neighbor input.
		return nullptr;
	}
}

GraphNode* Graph::getNeighborIgnorePortal(const GraphNode* node, const Direction& neighbor)
{
	switch (neighbor)
	{
	case Direction::LEFT:
		if (node->x == 0)
			return nullptr;

		return &m_nodeGrid[node->x - 1][node->y];
	case Direction::RIGHT:
		if (node->x == m_width - 1)
			return nullptr;

		return &m_nodeGrid[node->x + 1][node->y];
	case Direction::UP:
		if (node->y == 0)
			return nullptr;

		return &m_nodeGrid[node->x][node->y - 1];
	case Direction::DOWN:
		if (node->y == m_height - 1)
			return nullptr;

		return &m_nodeGrid[node->x][node->y + 1];
	default:
		assert(false); // Invalid neighbor input.
		return nullptr;
	}
}

bool Graph::hasNeighbor(const GraphNode* node, Direction neighbor)
{
	auto n = getNeighbor(node, neighbor);
	return n && n->valid;
}

GraphNode* Graph::getPortalNeighbor(uint8_t x, uint8_t y)
{
	return m_nodeGrid[x][y].valid ? m_nodeGrid[x][y].properties.otherPortal : nullptr;
}