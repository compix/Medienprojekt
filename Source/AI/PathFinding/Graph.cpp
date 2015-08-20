#include "Graph.h"
#include "../../LayerManager.h"
#include "../../GameConstants.h"
#include "../../Game.h"
#include "../../Components/BlockComponent.h"
#include "../../Components/SolidBlockComponent.h"
#include "../../Components/CellComponent.h"
#include "../../Components/BombComponent.h"
#include "../../Components/LayerComponent.h"
#include "../../Components/FloorComponent.h"
#include "../../Components/TimerComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/PortalComponent.h"
#include "../../Components/DestructionComponent.h"
#include "../../Events/ExitEvent.h"

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

	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		GameGlobals::events->emit<ExitEvent>();
		return;
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(16);
	m_text.setColor(sf::Color::White);
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
	resetCosts();
	m_normalBombs.clear();
	m_blocksAffectedByExplosion.clear();

	// Reset affectedByExplosion property
	for (auto x = 1; x < m_width - 1; ++x)
	{
		for (auto y = 1; y < m_height - 1; ++y)
		{
			m_nodeGrid[x][y].properties.affectedByExplosion = false;
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

	for (auto dieingBlock : GameGlobals::entities->entities_with_components<BlockComponent, DestructionComponent, CellComponent>())
	{
		if (!dieingBlock.valid())
			continue;

		auto cell = dieingBlock.component<CellComponent>();
		m_nodeGrid[cell->x][cell->y].properties.affectedByExplosion = true;
		m_blocksAffectedByExplosion.push_back(dieingBlock);
	}

	for (auto bombEntity : GameGlobals::entities->entities_with_components<BombComponent, TimerComponent, CellComponent>())
	{
		auto bombComponent = bombEntity.component<BombComponent>();
		auto timerComponent = bombEntity.component<TimerComponent>();
		auto cell = bombEntity.component<CellComponent>();

		//m_nodeGrid[cell->x][cell->y].properties.hasBomb = true;
		m_normalBombs.push_back(NormalBomb(cell->x, cell->y, bombComponent->explosionRange, timerComponent->seconds));
	}

	// Sort by explosionTime so that bombs that are affected by an explosion have their explosion time set to the "correct" value
	// This isn't perfect and should be reworked: 
	// 1. Get the bomb with the lowest explosion time. 
	// 2. Simulate the explosion and if it hits another bomb then simulate the explosion of the affected bomb if it's in the list.
	// 3. Remove the bombs that were already simulated from the list and go to 1. if the list isn't empty.
	std::sort(m_normalBombs.begin(), m_normalBombs.end(), [](const NormalBomb& b1, const NormalBomb& b2) {return b1.explosionTime < b2.explosionTime; });

	// Go through all explosion components and simulate the explosion.
	for (auto explosion : GameGlobals::entities->entities_with_components<ExplosionComponent, SpreadComponent, CellComponent>())
	{
		auto cell = explosion.component<CellComponent>();
		auto spread = explosion.component<SpreadComponent>();

		explosionSpread(cell->x, cell->y, spread->range, 0.f, spread->direction);
	}

	for (auto& bomb : m_normalBombs)
	{
		float exploTime = bomb.explosionTime;
		if (m_nodeGrid[bomb.x][bomb.y].properties.affectedByExplosion)
			exploTime = m_nodeGrid[bomb.x][bomb.y].properties.timeTillExplosion;

		placeBomb(bomb.x, bomb.y, bomb.range, exploTime);
	}
}

void Graph::explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction)
{
	auto currentNode = getNode(x, y);
	setOnFire(x, y, explosionTime);

	// Simulate the explosion:
	for (int j = 0; j < range; ++j)
	{
		currentNode = getNeighbor(currentNode, direction);
		explosionTime += GameConstants::EXPLOSION_SPREAD_TIME;

		if (currentNode->valid)
		{			
			setOnFire(currentNode->x, currentNode->y, explosionTime);
			if (currentNode->properties.hasItem)
			{
				m_nodeGrid[x][y].properties.numOfItemsAffectedByExplosion++;
				break; // Items stop explosions.
			}
		}
		else
		{
			if (currentNode->properties.hasBlock)
			{
				if (currentNode->properties.affectedByExplosion)
					m_nodeGrid[x][y].properties.numOfItemsAffectedByExplosion++;

				setOnFire(currentNode->x, currentNode->y, explosionTime);
				m_nodeGrid[x][y].properties.numOfBlocksAffectedByExplosion++;
				auto block = m_layerManager->getEntityWithComponent<BlockComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y);
				if (block.valid())
					m_blocksAffectedByExplosion.push_back(block);
			}

			// Doesn't really matter what else it is (might be a bomb for example). Just make it burn even if it's a solid block.
			setOnFire(currentNode->x, currentNode->y, explosionTime);

			// Explosion was stopped so get outta here.
			break;
		}
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

void Graph::placeBomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime)
{	
	auto currentNode = getNode(x, y);
	currentNode->properties.numOfBlocksAffectedByExplosion = 0;
	currentNode->properties.numOfPlayersAffectedByExplosion = 0;
	currentNode->properties.numOfItemsAffectedByExplosion = 0;

	for (int i = 0; i < 4; ++i) // Go in all directions
	{
		Direction direction = static_cast<Direction>(static_cast<int>(Direction::UP) + i);
		explosionSpread(x, y, range, explosionTime, direction);
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

void Graph::resetCosts()
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y].cost = 1;
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
	}

	if (entity.has_component<BlockComponent>())
	{
		removeNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasBlock = true;
		return;
	}

	if (entity.has_component<SolidBlockComponent>())
		removeNode(cell->x, cell->y);
}

void Graph::onEntityRemoved(entityx::Entity& entity)
{
	assert(entity.has_component<CellComponent>());
	auto cell = entity.component<CellComponent>();

	if (entity.has_component<InventoryComponent>())
	{
		m_nodeGrid[cell->x][cell->y].properties.hasPlayer = false;
		return;
	}

	if (entity.has_component<BombComponent>())
	{
		addNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasBomb = false;
		return;
	}
	
	if (entity.has_component<ItemComponent>())
	{
		m_nodeGrid[cell->x][cell->y].properties.hasItem = false;
		return;
	}

	if (entity.has_component<PortalComponent>())
	{
		m_nodeGrid[cell->x][cell->y].properties.hasPortal = false;
		m_nodeGrid[cell->x][cell->y].properties.otherPortal = nullptr;
		return;
	}

	if (entity.has_component<BlockComponent>())
	{
		addNode(cell->x, cell->y);
		m_nodeGrid[cell->x][cell->y].properties.hasBlock = false;
		return;
	}

	if (entity.has_component<SolidBlockComponent>())
		addNode(cell->x, cell->y);
}

void Graph::resetPathInfo(uint8_t taskNum)
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y].state[taskNum] = GraphNode::UNVISITED;
		}
	}
}

void Graph::visualize(bool nodes, bool pathInfo, bool dangerZones, bool properties)
{
	if (!nodes && !pathInfo && !dangerZones && !properties)
		return;

	m_circle.setRadius(20.f);
	m_circle.setOrigin(10, 10);
	m_circle.setFillColor(sf::Color(0, 255, 0, 50));

	if (properties)
	{
		// Draw the legend
		m_rect.setRotation(0.f);
		m_rect.setSize(sf::Vector2f(10, 10));

		m_rect.setFillColor(sf::Color(255, 0, 0));
		m_rect.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH, 25);
		m_text.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH + 12, 25);
		m_text.setString("Bomb");
		GameGlobals::window->draw(m_rect);
		GameGlobals::window->draw(m_text);

		m_rect.setFillColor(sf::Color(0, 255, 0));
		m_text.setString("Item");
		m_rect.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH, 50);
		m_text.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH + 12, 50);
		GameGlobals::window->draw(m_rect);
		GameGlobals::window->draw(m_text);

		m_rect.setFillColor(sf::Color(0, 0, 255));
		m_text.setString("Player");
		m_rect.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH, 75);
		m_text.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH + 12, 75);
		GameGlobals::window->draw(m_rect);
		GameGlobals::window->draw(m_text);

		m_rect.setFillColor(sf::Color(255, 255, 0));
		m_text.setString("Portal");
		m_rect.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH, 100);
		m_text.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH + 12, 100);
		GameGlobals::window->draw(m_rect);
		GameGlobals::window->draw(m_text);

		m_rect.setFillColor(sf::Color(139, 69, 19));
		m_text.setString("Block");
		m_rect.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH, 125);
		m_text.setPosition(m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH + 12, 125);
		GameGlobals::window->draw(m_rect);
		GameGlobals::window->draw(m_text);
	}

	for (auto x = 1; x < m_width - 1; ++x)
	{
		for (auto y = 1; y < m_height - 1; ++y)
		{
			auto& node = m_nodeGrid[x][y];

			m_circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 5.f);

			if (nodes && node.valid)
			{
				m_circle.setFillColor(sf::Color(0, 255, 0, 40));
				GameGlobals::window->draw(m_circle);
			}

			m_circle.setFillColor(sf::Color(0, 255, 0, 50));

			if (pathInfo && node.valid)
			{
				switch (node.state[0])
				{
				case GraphNode::UNVISITED:
					m_circle.setFillColor(sf::Color(0, 255, 0, 50));
					break;
				case GraphNode::OPEN:
					m_circle.setFillColor(sf::Color(255, 255, 0, 50));
					break;
				case GraphNode::CLOSED:
					m_circle.setFillColor(sf::Color(255, 0, 255, 50));
					break;
				default: break;
				}

				m_rect.setSize(sf::Vector2f(2, GameConstants::CELL_HEIGHT));
				m_rect.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f + 7.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 13.f);

				if (hasNeighbor(&node, Direction::LEFT))
				{
					m_rect.setRotation(90);
					GameGlobals::window->draw(m_rect);
				}

				if (hasNeighbor(&node, Direction::RIGHT))
				{
					m_rect.setRotation(-90);
					GameGlobals::window->draw(m_rect);
				}

				if (hasNeighbor(&node, Direction::UP))
				{
					m_rect.setRotation(180);
					GameGlobals::window->draw(m_rect);
				}

				if (hasNeighbor(&node, Direction::DOWN))
				{
					m_rect.setRotation(0);
					GameGlobals::window->draw(m_rect);
				}

				GameGlobals::window->draw(m_circle);
			}


			if (dangerZones && node.properties.affectedByExplosion)
			{
				m_circle.setFillColor(sf::Color(100 * node.properties.timeTillExplosion, 0, 0, 150));
				GameGlobals::window->draw(m_circle);
			}

			if (properties)
			{
				m_rect.setRotation(0.f);

				int propertyNum = 0;
				m_rect.setSize(sf::Vector2f(5, GameConstants::CELL_HEIGHT));
				if (node.properties.hasBomb)
				{	
					m_rect.setFillColor(sf::Color(255, 0, 0));
					m_rect.setPosition(x * GameConstants::CELL_WIDTH + propertyNum * 5, y * GameConstants::CELL_HEIGHT);
					GameGlobals::window->draw(m_rect);
					propertyNum++;
				}

				if (node.properties.hasItem)
				{
					m_rect.setFillColor(sf::Color(0, 255, 0));
					m_rect.setPosition(x * GameConstants::CELL_WIDTH + propertyNum * 5, y * GameConstants::CELL_HEIGHT);
					GameGlobals::window->draw(m_rect);
					propertyNum++;
				}

				if (node.properties.hasPlayer)
				{
					m_rect.setFillColor(sf::Color(0, 0, 255));
					m_rect.setPosition(x * GameConstants::CELL_WIDTH + propertyNum * 5, y * GameConstants::CELL_HEIGHT);
					GameGlobals::window->draw(m_rect);
					propertyNum++;
				}

				if (node.properties.hasPortal)
				{
					m_rect.setFillColor(sf::Color(255, 255, 0));
					m_rect.setPosition(x * GameConstants::CELL_WIDTH + propertyNum * 5, y * GameConstants::CELL_HEIGHT);
					GameGlobals::window->draw(m_rect);
					propertyNum++;
				}

				if (node.properties.hasBlock)
				{
					m_rect.setFillColor(sf::Color(139, 69, 19));
					m_rect.setPosition(x * GameConstants::CELL_WIDTH + propertyNum * 5, y * GameConstants::CELL_HEIGHT);
					GameGlobals::window->draw(m_rect);
					propertyNum++;
				}
			}
		}
	}
}

GraphNode* Graph::getNeighbor(const GraphNode* node, const Direction& neighbor)
{
	assert(node->x > 0 && node->x < m_width - 1 && node->y > 0 && node->y < m_height - 1);

	GraphNode* portalNode;
	switch (neighbor)
	{
	case Direction::LEFT:
		portalNode = getOtherPortalNode(node->x - 1, node->y);
		return portalNode ? portalNode : &m_nodeGrid[node->x - 1][node->y];
	case Direction::RIGHT:
		portalNode = getOtherPortalNode(node->x + 1, node->y);
		return portalNode ? portalNode : &m_nodeGrid[node->x + 1][node->y];
	case Direction::UP:
		portalNode = getOtherPortalNode(node->x, node->y - 1);
		return portalNode ? portalNode : &m_nodeGrid[node->x][node->y - 1];
	case Direction::DOWN:
		portalNode = getOtherPortalNode(node->x, node->y + 1);
		return portalNode ? portalNode : &m_nodeGrid[node->x][node->y + 1];
	default: 
		assert(false);
		return nullptr;
	}
}

bool Graph::hasNeighbor(const GraphNode* node, Direction neighbor)
{
	return getNeighbor(node, neighbor)->valid;
}

GraphNode* Graph::getOtherPortalNode(uint8_t x, uint8_t y)
{
	if (m_nodeGrid[x][y].properties.otherPortal)
		return m_nodeGrid[x][y].properties.otherPortal;

	return m_nodeGrid[x][y].properties.otherPortal;
}