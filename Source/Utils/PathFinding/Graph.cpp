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

Graph::Graph(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
	auto mainLayer = m_layerManager->getLayer(GameConstants::MAIN_LAYER);
	m_width = mainLayer->getWidth();
	m_height = mainLayer->getHeight();

	m_nodeGrid = new GraphNode*[m_width];
	for (int i = 0; i < m_width; i++)
		m_nodeGrid[i] = new GraphNode[m_height];
}

Graph::~Graph()
{
	for (int i = 0; i < m_width; i++)
		delete[] m_nodeGrid[i];

	delete[] m_nodeGrid;
}

void Graph::addNode(uint8_t x, uint8_t y)
{
	if (x < 1 || x > m_width - 2 || y < 1 || y > m_height - 2)
		return;

	auto& node = m_nodeGrid[x][y];
	node.valid = true;
	node.x = x;
	node.y = y;
	node.cost = 1;
}

void Graph::removeNode(uint8_t x, uint8_t y)
{
	if (x < 1 || x > m_width - 2 ||y < 1 || y > m_height - 2)
		return;

	m_nodeGrid[x][y].valid = false;
}

void Graph::onEntityAdded(entityx::Entity entity)
{
	assert(entity.has_component<CellComponent>() && entity.has_component<LayerComponent>());

	if (entity.component<LayerComponent>()->layer != GameConstants::MAIN_LAYER)
		return;

	if (entity.has_component<BombComponent>())
	{
		auto cell = entity.component<CellComponent>();

		removeNode(cell->x, cell->y);
	}
}

void Graph::onEntityRemoved(entityx::Entity entity)
{
	assert(entity.has_component<CellComponent>() && entity.has_component<LayerComponent>());

	if (entity.component<LayerComponent>()->layer != GameConstants::MAIN_LAYER)
		return;

	if (entity.has_component<BlockComponent>() || entity.has_component<BombComponent>())
	{
		auto cell = entity.component<CellComponent>();

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

			for (auto& e : entities)
				if (e.component<BlockComponent>() || e.component<SolidBlockComponent>())
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

			if (hasNeighbor(&node, GraphNode::LEFT))
			{
				rect.setRotation(90);
				GameGlobals::window->draw(rect);
			}

			if (hasNeighbor(&node, GraphNode::RIGHT))
			{
				rect.setRotation(-90);
				GameGlobals::window->draw(rect);
			}

			if (hasNeighbor(&node, GraphNode::TOP))
			{
				rect.setRotation(180);
				GameGlobals::window->draw(rect);
			}

			if (hasNeighbor(&node, GraphNode::BOT))
			{
				rect.setRotation(0);
				GameGlobals::window->draw(rect);
			}

			GameGlobals::window->draw(circle);
		}
	}
}

GraphNode* Graph::getNeighbor(GraphNode* node, const GraphNode::Neighbor& neighbor)
{
	assert(node->x > 0 && node->x < m_width - 1 && node->y > 0 && node->y < m_height - 1);

	switch (neighbor)
	{
	case GraphNode::LEFT: 
		return &m_nodeGrid[node->x - 1][node->y];
		break;
	case GraphNode::RIGHT:
		return &m_nodeGrid[node->x + 1][node->y];
		break;
	case GraphNode::TOP:
		return &m_nodeGrid[node->x][node->y - 1];
		break;
	case GraphNode::BOT:
		return &m_nodeGrid[node->x][node->y + 1];
		break;
	default: 
		assert(false);
		return nullptr;
	}
}

bool Graph::hasNeighbor(GraphNode* node, GraphNode::Neighbor neighbor)
{
	return getNeighbor(node, neighbor)->valid;
}