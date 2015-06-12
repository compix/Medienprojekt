#include "PathEngine.h"
#include <queue>
#include <unordered_map>
#include <set>
#include "../../Game.h"

PathEngine::PathEngine(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
	m_graph = std::make_unique<Graph>(layerManager);

	m_startDummy.prev = nullptr;
	m_endDummy.next = nullptr;
}
void PathEngine::computePath(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, Path& pathOut)
{
	auto goal = m_graph->getNode(endX, endY);

	GraphNode* start = m_graph->getNode(startX, startY);
	start->costSoFar = 0;
	start->estimatedTotalCost = estimate(start, goal);
	start->prevOnPath = nullptr;
	start->next = &m_endDummy;
	start->prev = &m_startDummy;
	m_startDummy.next = start;

	m_graph->resetPathInfo();

	bool pathFound = false;
	GraphNode* minNode = start;

	while (minNode != &m_endDummy)
	{
		// close this node
		minNode->state = GraphNode::CLOSED;

		
		if (minNode == goal)
		{
			pathFound = true;
			break;
		}

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			auto neighbor = m_graph->getNeighbor(minNode, static_cast<Direction>(i));

			if (!neighbor->valid)
				continue;

			switch (neighbor->state)
			{
			case GraphNode::UNVISITED:
				// New node - Initialize it.
				neighbor->costSoFar = minNode->costSoFar + neighbor->cost;
				neighbor->estimatedTotalCost = neighbor->costSoFar + estimate(neighbor, goal);
				neighbor->prevOnPath = minNode;
				neighbor->state = GraphNode::OPEN;
				insert(neighbor, minNode);
				break;
			case GraphNode::OPEN:
				// update/relax the node if necessary
				if (minNode->costSoFar + neighbor->cost + estimate(neighbor, goal) < neighbor->estimatedTotalCost)
				{
					neighbor->costSoFar = minNode->costSoFar + neighbor->cost;
					neighbor->estimatedTotalCost = neighbor->costSoFar + estimate(neighbor, goal);
					neighbor->prevOnPath = minNode;
					// update list position
					auto at = remove(neighbor);
					insert(neighbor, at);
				}
				break;
			case GraphNode::CLOSED: break;
			default: break;
			}
		}
	
		remove(minNode);
		// Get the next min
		minNode = m_startDummy.next;
	}

	pathOut.nodeCount = 0;
	pathOut.cost = 0;

	if (pathFound)
		makePath(pathOut, minNode);
}

void PathEngine::visualize()
{
	m_graph->visualize();
}

void PathEngine::visualize(Path& path)
{
	sf::CircleShape circle(20);
	circle.setOrigin(10, 10);

	for (uint16_t i = 0; i < path.nodeCount; ++i)
	{
		uint8_t x = path.nodes[i]->x;
		uint8_t y = path.nodes[i]->y;

		circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_WIDTH*0.5f + 5.f);
		circle.setFillColor(sf::Color(0, 0, 255, 50));

		GameGlobals::window->draw(circle);
	}
}

void PathEngine::update()
{
	m_graph->update();
}

uint32_t PathEngine::estimate(GraphNode* node, GraphNode* goal)
{
	return (abs(node->x - goal->x) + abs(node->y - goal->y)) * NodeCost::NORMAL;
}

void PathEngine::insert(GraphNode* newNode, GraphNode* at)
{
	// Requires to have a dummy at the start and end of the list
	assert(newNode && at && at->next && at->prev);

	auto cur = at;

	while (cur->next->next && newNode->estimatedTotalCost > cur->next->estimatedTotalCost)
		cur = cur->next;

	while (cur->prev && newNode->estimatedTotalCost < cur->estimatedTotalCost)
		cur = cur->prev;

	// found correct position -> place newNode after the current node and adjust pointers accordingly
	auto n = cur->next;
	cur->next = newNode;
	newNode->next = n;
	newNode->prev = cur;
	n->prev = newNode;
}

GraphNode* PathEngine::remove(GraphNode* node)
{
	assert(node);
	node->prev->next = node->next;
	node->next->prev = node->prev;
	return node->prev;
}

void PathEngine::makePath(Path& pathOut, GraphNode* goal)
{
	auto cur = goal;

	pathOut.nodeCount = 0;
	pathOut.cost = goal->costSoFar;

	// count path nodes to get the correct order
	uint16_t num = 0;
	while (cur)
	{
		++num;
		cur = cur->prevOnPath;
	}

	cur = goal;
	while (cur)
	{
		pathOut.nodes[--num] = cur;
		++pathOut.nodeCount;
		cur = cur->prevOnPath;
	}
}