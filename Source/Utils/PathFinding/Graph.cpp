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
}

Graph::~Graph()
{
	for (int i = 0; i < m_width; i++)
	{
		delete[] m_nodeGrid[i];
	}
		
	delete[] m_nodeGrid;
}

void Graph::timeTravel(float seconds, float t)
{
	resetCosts();
	resetProperties();
	m_normalBombs.clear();

	for (auto& bomb : GameGlobals::entities->entities_with_components<BombComponent, TimerComponent, CellComponent>())
	{	
		auto bombComponent = bomb.component<BombComponent>();
		auto timerComponent = bomb.component<TimerComponent>();
		auto cell = bomb.component<CellComponent>();

		m_nodeGrid[cell->x][cell->y].properties.hasBomb = true;
		m_normalBombs.push_back(NormalBomb(cell->x, cell->y, bombComponent->explosionRange, timerComponent->seconds));
	}

	std::sort(m_normalBombs.begin(), m_normalBombs.end(), [](const NormalBomb& b1, const NormalBomb& b2) {return b1.explosionTime < b2.explosionTime; });

	// Go through all explosion components and simulate the explosion.
	for (auto& explosion : GameGlobals::entities->entities_with_components<ExplosionComponent, SpreadComponent, CellComponent>())
	{
		auto cell = explosion.component<CellComponent>();
		auto spread = explosion.component<SpreadComponent>();

		explosionSpread(cell->x, cell->y, spread->range, 0.f, spread->direction, seconds);
	}

	for (auto& bomb : m_normalBombs)
	{
		float exploTime = bomb.explosionTime;
		if (m_nodeGrid[bomb.x][bomb.y].properties.affectedByExplosion)
			exploTime = m_nodeGrid[bomb.x][bomb.y].properties.timeTillExplosion;

		placeBomb(bomb.x, bomb.y, bomb.range, exploTime, seconds);
	}

	for (auto& item : GameGlobals::entities->entities_with_components<ItemComponent, CellComponent>())
	{
		auto cell = item.component<CellComponent>();
		m_nodeGrid[cell->x][cell->y].properties.isItem = true;
	}
}

void Graph::update(float deltaTime)
{
	resetCosts();
	resetProperties();
	m_normalBombs.clear();

	for (auto& bomb : GameGlobals::entities->entities_with_components<BombComponent, TimerComponent, CellComponent>())
	{
		auto bombComponent = bomb.component<BombComponent>();
		auto timerComponent = bomb.component<TimerComponent>();
		auto cell = bomb.component<CellComponent>();

		m_nodeGrid[cell->x][cell->y].properties.hasBomb = true;
		m_normalBombs.push_back(NormalBomb(cell->x, cell->y, bombComponent->explosionRange, timerComponent->seconds));
	}

	std::sort(m_normalBombs.begin(), m_normalBombs.end(), [](const NormalBomb& b1, const NormalBomb& b2) {return b1.explosionTime < b2.explosionTime; });

	// Go through all explosion components and simulate the explosion.
	for (auto& explosion : GameGlobals::entities->entities_with_components<ExplosionComponent, SpreadComponent, CellComponent>())
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

	for (auto& item : GameGlobals::entities->entities_with_components<ItemComponent, CellComponent>())
	{
		auto cell = item.component<CellComponent>();
		m_nodeGrid[cell->x][cell->y].properties.isItem = true;
	}
}

void Graph::explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction, float futureTime)
{
	float futureExplosionTime = explosionTime + range * GameConstants::EXPLOSION_SPREAD_TIME - futureTime;

	if (futureExplosionTime <= 0.f)
		return;

	auto currentNode = getNode(x, y);
	float currentExplosionTime = currentNode->properties.timeTillExplosion;
	futureExplosionTime = explosionTime - futureTime;
	currentNode->properties.timeTillExplosion = currentNode->properties.affectedByExplosion ? std::min(futureExplosionTime, currentExplosionTime) : futureExplosionTime;
	currentNode->properties.affectedByExplosion = true;

	// Simulate the explosion:
	for (int j = 0; j < range; ++j)
	{
		currentNode = getNeighbor(currentNode, direction);
		futureExplosionTime = explosionTime + (j + 1) * GameConstants::EXPLOSION_SPREAD_TIME - futureTime;
		currentExplosionTime = currentNode->properties.timeTillExplosion;

		if (currentNode->valid)
		{				
			currentNode->properties.timeTillExplosion = currentNode->properties.affectedByExplosion ? std::min(futureExplosionTime, currentExplosionTime) : futureExplosionTime;
			currentNode->properties.affectedByExplosion = true;

			if (m_layerManager->hasEntityWithComponent<ItemComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y))
			{
				m_nodeGrid[x][y].properties.numOfItemsAffectedByExplosion++;
			}
		}
		else
		{
			if (m_layerManager->hasEntityWithComponents<BlockComponent, HealthComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y))
			{
				if (currentNode->properties.affectedByExplosion)
					m_nodeGrid[x][y].properties.numOfItemsAffectedByExplosion++;

				m_nodeGrid[x][y].properties.numOfBlocksAffectedByExplosion++;
				currentNode->properties.affectedByExplosion = true;
			}

			if (m_layerManager->hasEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y))
			{
				currentNode->properties.timeTillExplosion = currentNode->properties.affectedByExplosion ? std::min(futureExplosionTime, currentExplosionTime) : futureExplosionTime;
				currentNode->properties.affectedByExplosion = true;
			}

			break;
		}
	}
}

void Graph::placeBomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime, float futureTime)
{	
	auto currentNode = getNode(x, y);
	currentNode->properties.numOfBlocksAffectedByExplosion = 0;
	currentNode->properties.numOfPlayersAffectedByExplosion = 0;
	currentNode->properties.numOfItemsAffectedByExplosion = 0;

	for (int i = 0; i < 4; ++i) // Go in all directions
	{
		Direction direction = static_cast<Direction>(static_cast<int>(Direction::UP) + i);
		explosionSpread(x, y, range, explosionTime, direction, futureTime);
	}
}

void Graph::removeBomb(uint8_t x, uint8_t y, uint8_t range)
{
	// Reverse the explosion simulation
	for (int i = 0; i < 4; ++i) // Go in all directions
	{
		Direction currentNeighbor = static_cast<Direction>(static_cast<int>(Direction::UP) + i);
		auto currentNode = getNode(x, y);
		currentNode->properties.affectedByExplosion = false;

		for (int j = 0; j < range; ++j)
		{
			currentNode = getNeighbor(currentNode, currentNeighbor);

			if (currentNode->valid)
			{
				currentNode->properties.affectedByExplosion = false;
			}
			else
				break;
		}
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

void Graph::resetProperties()
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y].properties.affectedByExplosion = false;
			m_nodeGrid[x][y].properties.isItem = false;
			m_nodeGrid[x][y].properties.hasBomb = false;
			//m_nodeGrid[x][y].properties.marked = false;
		}
	}
}

void Graph::addNode(uint8_t x, uint8_t y)
{
	if (x < 1 || x > m_width - 2 || y < 1 || y > m_height - 2)
		return;

	auto& node = m_nodeGrid[x][y];
	node.valid = true;
	node.x = x;
	node.y = y;
}

void Graph::removeNode(uint8_t x, uint8_t y)
{
	if (x < 1 || x > m_width - 2 ||y < 1 || y > m_height - 2)
		return;

	m_nodeGrid[x][y].valid = false;
}

void Graph::onEntityAdded(entityx::Entity& entity)
{
	assert(entity.has_component<CellComponent>());

	if (entity.has_component<BombComponent>())
	{
		auto cell = entity.component<CellComponent>();
		removeNode(cell->x, cell->y);
	}

	if (entity.has_component<ItemComponent>())
	{
		auto cell = entity.component<CellComponent>();
	}
}

void Graph::onEntityRemoved(entityx::Entity& entity)
{
	assert(entity.has_component<CellComponent>());

	if (entity.has_component<BlockComponent>() || entity.has_component<BombComponent>())
	{
		auto cell = entity.component<CellComponent>();
		addNode(cell->x, cell->y);
	}
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

void Graph::init()
{
	auto mainLayer = m_layerManager->getLayer(GameConstants::MAIN_LAYER);

	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			auto& entities = mainLayer->get(x, y);
			bool free = true;
			m_nodeGrid[x][y].valid = false;
			m_nodeGrid[x][y].x = x;
			m_nodeGrid[x][y].y = y;

			for (auto& e : entities)
				if (e.component<BlockComponent>() || e.component<SolidBlockComponent>())
					free = false;

			if (free)
				addNode(x, y);
		}
	}
}

void Graph::visualize(bool nodes, bool smells)
{
	if (nodes)
	{
		sf::CircleShape circle(20);
		circle.setOrigin(10, 10);
		circle.setFillColor(sf::Color(0, 255, 0, 50));

		sf::RectangleShape rect(sf::Vector2f(2, GameConstants::CELL_HEIGHT));
		rect.setFillColor(sf::Color(0, 255, 0, 40));

		for (auto x = 0; x < m_width; ++x)
		{
			for (auto y = 0; y < m_height; ++y)
			{
				auto& node = m_nodeGrid[x][y];
				if (!node.valid)
					continue;

				circle.setFillColor(sf::Color(0, 255, 0, 50));

				if (node.properties.affectedByExplosion)
					circle.setFillColor(sf::Color(100 * node.properties.timeTillExplosion, 0, 0, 100));

				/*
				switch (node.state)
				{
				case GraphNode::UNVISITED:
					circle.setFillColor(sf::Color(0, 255, 0, 50));
					break;
				case GraphNode::OPEN:
					circle.setFillColor(sf::Color(255, 255, 0, 50));
					break;
				case GraphNode::CLOSED:
					circle.setFillColor(sf::Color(255, 0, 0, 50));
					break;
				default: break;
				}*/

				circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 5.f);
				rect.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f + 7.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 13.f);

				if (hasNeighbor(&node, Direction::LEFT))
				{
					rect.setRotation(90);
					GameGlobals::window->draw(rect);
				}

				if (hasNeighbor(&node, Direction::RIGHT))
				{
					rect.setRotation(-90);
					GameGlobals::window->draw(rect);
				}

				if (hasNeighbor(&node, Direction::UP))
				{
					rect.setRotation(180);
					GameGlobals::window->draw(rect);
				}

				if (hasNeighbor(&node, Direction::DOWN))
				{
					rect.setRotation(0);
					GameGlobals::window->draw(rect);
				}

				GameGlobals::window->draw(circle);
			}
		}
	}
}

GraphNode* Graph::getNeighbor(const GraphNode* node, const Direction& neighbor)
{
	assert(node->x > 0 && node->x < m_width - 1 && node->y > 0 && node->y < m_height - 1);

	switch (neighbor)
	{
	case Direction::LEFT:
		return &m_nodeGrid[node->x - 1][node->y];
		break;
	case Direction::RIGHT:
		return &m_nodeGrid[node->x + 1][node->y];
		break;
	case Direction::UP:
		return &m_nodeGrid[node->x][node->y - 1];
		break;
	case Direction::DOWN:
		return &m_nodeGrid[node->x][node->y + 1];
		break;
	default: 
		assert(false);
		return nullptr;
	}
}

bool Graph::hasNeighbor(const GraphNode* node, Direction neighbor)
{
	return getNeighbor(node, neighbor)->valid;
}