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

Graph::Graph(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
	auto mainLayer = m_layerManager->getLayer(GameConstants::MAIN_LAYER);
	m_width = mainLayer->getWidth();
	m_height = mainLayer->getHeight();

	m_nodeGrid = new GraphNode*[m_width];
	for (int i = 0; i < m_width; i++)
		m_nodeGrid[i] = new GraphNode[m_height];

	mainLayer->listen(this);
}

Graph::~Graph()
{
	for (int i = 0; i < m_width; i++)
		delete[] m_nodeGrid[i];

	delete[] m_nodeGrid;
}

void Graph::update()
{
	resetCosts();

	// Go through all bombs and mark danger zones. Note: For correct danger estimations the AI should be considered and every AI should have an own graph like this.
	for (auto bomb : GameGlobals::entities->entities_with_components<BombComponent, TimerComponent, CellComponent>())
	{
		auto bombComponent = bomb->get<BombComponent>();
		auto timerComponent = bomb->get<TimerComponent>();
		auto cell = bomb->get<CellComponent>();

		auto correspondingNode = getNode(cell->x, cell->y);

		// Simulate the explosion:
		for (int i = 0; i < 4; ++i) // Go in all directions
		{
			Direction currentNeighbor = static_cast<Direction>(static_cast<int>(Direction::UP) + i);
			auto currentNode = correspondingNode;
			currentNode->cost = estimateDangerCost(timerComponent->seconds);

			for (int j = 0; j < bombComponent->explosionRange; ++j)
			{
				currentNode = getNeighbor(currentNode, currentNeighbor);

				if (currentNode->valid)
					currentNode->cost = estimateDangerCost(timerComponent->seconds);
				else
					break;			
			}
		}
	}

	// Go through all explosion components and simulate the explosion.
	for (auto explosion : GameGlobals::entities->entities_with_components<ExplosionComponent, SpreadComponent, CellComponent>())
	{
		auto cell = explosion->get<CellComponent>();
		auto spread = explosion->get<SpreadComponent>();

		auto currentNode = getNode(cell->x, cell->y);
		currentNode->cost = NodeCost::DANGER_HIGH;

		for (int j = 0; j < spread->range; ++j)
		{
			currentNode = getNeighbor(currentNode, spread->direction);

			if (currentNode->valid)
				currentNode->cost = NodeCost::DANGER_HIGH;
			else
				break;
		}
	}
}

uint32_t Graph::estimateDangerCost(float remainingTimeTillExplosion)
{
	if (remainingTimeTillExplosion >= 2.f)
		return NodeCost::DANGER_LOW;
	else if (remainingTimeTillExplosion >= 1.f)
		return NodeCost::DANGER_NORMAL;
	else
		return NodeCost::DANGER_HIGH;
}

void Graph::resetCosts()
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y].cost = NodeCost::NORMAL;
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

void Graph::onEntityAdded(Entity *entity)
{
	assert(entity->has<CellComponent>());

	if (entity->has<BombComponent>())
	{
		auto cell = entity->get<CellComponent>();
		removeNode(cell->x, cell->y);
	}
}

void Graph::onEntityRemoved(Entity *entity)
{
	assert(entity->has<CellComponent>());

	if (entity->has<BlockComponent>() || entity->has<BombComponent>())
	{
		auto cell = entity->get<CellComponent>();
		addNode(cell->x, cell->y);
	}
}

void Graph::resetPathInfo()
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y].state = GraphNode::UNVISITED;
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
				if (e->get<BlockComponent>() || e->get<SolidBlockComponent>())
					free = false;

			if (free)
				addNode(x, y);
		}
	}
}

void Graph::visualize()
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
			}

			circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_WIDTH*0.5f + 5.f);
			rect.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f + 7.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_WIDTH*0.5f + 13.f);

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