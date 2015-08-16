#include "PathEngine.h"
#include <queue>
#include <unordered_map>
#include <set>
#include "../../Game.h"
#include "../../Components/BombComponent.h"
#include "../../Components/BlockComponent.h"
#include "SimulationGraph.h"
#include "../../Components/CellComponent.h"
#include "../../Components/PortalComponent.h"
#include "AIPath.h"

PathEngine::PathEngine(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
	m_graph = std::make_unique<Graph>(layerManager);
	m_simGraph = std::make_unique<SimulationGraph>(layerManager, m_graph.get());

	m_startDummy.prev[0] = nullptr;
	m_endDummy.next[0] = nullptr;

	m_startDummy.prev[1] = nullptr;
	m_endDummy.next[1] = nullptr;
}

void PathEngine::computePath(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, AIPath& pathOut, uint8_t taskNum)
{
	auto goal = m_graph->getNode(endX, endY);

	GraphNode* start = m_graph->getNode(startX, startY);
	start->costSoFar = 0;
	start->estimatedTotalCost = estimate(start, goal);
	start->prevOnPath[taskNum] = nullptr;
	start->next[taskNum] = &m_endDummy;
	start->prev[taskNum] = &m_startDummy;
	m_startDummy.next[taskNum] = start;

	m_graph->resetPathInfo(taskNum);

	bool pathFound = false;
	GraphNode* minNode = start;

	while (minNode != &m_endDummy)
	{
		// close this node
		minNode->state[taskNum] = GraphNode::CLOSED;

		
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

			switch (neighbor->state[taskNum])
			{
			case GraphNode::UNVISITED:
				// New node - Initialize it.
				neighbor->costSoFar = minNode->costSoFar + neighbor->cost;
				neighbor->estimatedTotalCost = neighbor->costSoFar + estimate(neighbor, goal);
				neighbor->prevOnPath[taskNum] = minNode;
				neighbor->state[taskNum] = GraphNode::OPEN;
				insert(neighbor, minNode, taskNum);
				break;
			case GraphNode::OPEN:
				// update/relax the node if necessary
				if (minNode->costSoFar + neighbor->cost + estimate(neighbor, goal) < neighbor->estimatedTotalCost)
				{
					neighbor->costSoFar = minNode->costSoFar + neighbor->cost;
					neighbor->estimatedTotalCost = neighbor->costSoFar + estimate(neighbor, goal);
					neighbor->prevOnPath[taskNum] = minNode;
					// update list position
					auto at = remove(neighbor, taskNum);
					insert(neighbor, at, taskNum);
				}
				break;
			case GraphNode::CLOSED: break;
			default: break;
			}
		}
	
		remove(minNode, taskNum);
		// Get the next min
		minNode = m_startDummy.next[taskNum];
	}

	if (pathFound)
		makePath(pathOut, minNode, taskNum);
}

void PathEngine::breadthFirstSearch(uint8_t startX, uint8_t startY, NodeType targetType, AIPath& pathOut, uint8_t taskNum)
{
	m_simGraph->resetPathInfo(taskNum);

	std::queue<GraphNode*> processQueue;
	auto startNode = m_simGraph->getNode(startX, startY);
	startNode->prevOnPath[taskNum] = nullptr;
	processQueue.push(startNode);
	startNode->state[taskNum] = GraphNode::CLOSED;

	while (processQueue.size() > 0)
	{
		GraphNode* curNode = processQueue.front();
		processQueue.pop();

		bool goalReached = false;

		switch (targetType)
		{
		case NodeType::SAFE:
			if (!curNode->properties.affectedByExplosion)
				goalReached = true;
			break;
		case NodeType::SAFE_NOT_MARKED:
			if (!curNode->properties.affectedByExplosion && !curNode->marked)
				goalReached = true;
			break;
		case NodeType::FREE:
			if (!curNode->marked && curNode->valid)
				goalReached = true;
			break;
		case NodeType::ITEM:
			if (curNode->properties.isItem && !curNode->marked)
				goalReached = true;
			break;
		default:
			// Should never happen
			assert(false);
			break;
		}

		if (goalReached)
		{
			makePath(pathOut, curNode, taskNum);
			return;
		}

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighbor = m_simGraph->getNeighbor(curNode, static_cast<Direction>(i));
			if (!neighbor->valid)
				continue;
			
			if (neighbor->state[taskNum] == GraphNode::UNVISITED)
			{
				neighbor->prevOnPath[taskNum] = curNode;
				neighbor->state[taskNum] = GraphNode::CLOSED;
				processQueue.push(neighbor);	
			}
		}
	}

	pathOut.nodes.clear();
}

void PathEngine::breadthFirstSearch(uint8_t startX, uint8_t startY, AIPath& pathOut, PathRatingFunction ratePath, uint8_t taskNum)
{
	m_simGraph->resetPathInfo(taskNum);

	std::queue<GraphNode*> processQueue;
	auto startNode = m_simGraph->getNode(startX, startY);
	startNode->prevOnPath[taskNum] = nullptr;
	processQueue.push(startNode);
	startNode->state[taskNum] = GraphNode::CLOSED;

	while (processQueue.size() > 0)
	{
		GraphNode* curNode = processQueue.front();
		processQueue.pop();

		if (ratePath(this, curNode, pathOut, taskNum))
			return;

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighbor = m_simGraph->getNeighbor(curNode, static_cast<Direction>(i));
			if (!neighbor->valid)
				continue;

			if (neighbor->state[taskNum] == GraphNode::UNVISITED)
			{
				neighbor->prevOnPath[taskNum] = curNode;
				neighbor->state[taskNum] = GraphNode::CLOSED;
				processQueue.push(neighbor);
			}
		}
	}

	pathOut.nodes.clear();
}

void PathEngine::searchBest(uint8_t startX, uint8_t startY, AIPath& pathOut, PathRatingFunction ratePath, uint8_t maxChecks, uint8_t taskNum)
{
	assert(maxChecks > 0);

	m_simGraph->resetPathInfo(taskNum);

	std::queue<GraphNode*> processQueue;
	auto startNode = m_simGraph->getNode(startX, startY);
	startNode->prevOnPath[taskNum] = nullptr;
	processQueue.push(startNode);
	startNode->state[taskNum] = GraphNode::CLOSED;
	AIPath path;
	AIPath bestPath;

	while (processQueue.size() > 0)
	{
		GraphNode* curNode = processQueue.front();
		processQueue.pop();

		if (ratePath(this, curNode, path, taskNum))
		{		
			maxChecks--;
			if (path.rating > bestPath.rating)
				bestPath = path;
		}

		if (maxChecks == 0)
			break;

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighbor = m_simGraph->getNeighbor(curNode, static_cast<Direction>(i));
			if (!neighbor->valid)
				continue;

			if (neighbor->state[taskNum] == GraphNode::UNVISITED)
			{
				neighbor->prevOnPath[taskNum] = curNode;
				neighbor->state[taskNum] = GraphNode::CLOSED;
				processQueue.push(neighbor);
			}
		}
	}

	pathOut.nodes.clear();
	pathOut.rating = bestPath.rating;
	for (int i = 0; i < bestPath.nodes.size(); ++i)
		pathOut.nodes.push_back(bestPath.nodes[i]);
}

void PathEngine::visualize()
{
	m_graph->visualize(true, true);
}

void PathEngine::visualize(AIPath& path)
{
	sf::CircleShape circle(20);
	circle.setOrigin(10, 10);

	for (uint16_t i = 0; i < path.nodes.size(); ++i)
	{
		uint8_t x = path.nodes[i]->x;
		uint8_t y = path.nodes[i]->y;

		circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_WIDTH*0.5f + 5.f);
		circle.setFillColor(sf::Color(0, 0, 255, 50));

		GameGlobals::window->draw(circle);
	}
}

void PathEngine::update(float deltaTime)
{
	m_graph->update(deltaTime);
	m_simGraph->update(deltaTime);
}

uint32_t PathEngine::estimate(GraphNode* node, GraphNode* goal)
{
	return (abs(node->x - goal->x) + abs(node->y - goal->y));
}

void PathEngine::insert(GraphNode* newNode, GraphNode* at, uint8_t taskNum)
{
	// Requires to have a dummy at the start and end of the list
	assert(newNode && at && at->next[taskNum] && at->prev[taskNum]);

	auto cur = at;

	while (cur->next[taskNum]->next[taskNum] && newNode->estimatedTotalCost > cur->next[taskNum]->estimatedTotalCost)
		cur = cur->next[taskNum];

	while (cur->prev[taskNum] && newNode->estimatedTotalCost < cur->estimatedTotalCost)
		cur = cur->prev[taskNum];

	// found correct position -> place newNode after the current node and adjust pointers accordingly
	auto n = cur->next[taskNum];
	cur->next[taskNum] = newNode;
	newNode->next[taskNum] = n;
	newNode->prev[taskNum] = cur;
	n->prev[taskNum] = newNode;
}

GraphNode* PathEngine::remove(GraphNode* node, uint8_t taskNum)
{
	assert(node);
	node->prev[taskNum]->next[taskNum] = node->next[taskNum];
	node->next[taskNum]->prev[taskNum] = node->prev[taskNum];
	return node->prev[taskNum];
}

void PathEngine::makePath(AIPath& pathOut, GraphNode* goal, uint8_t taskNum)
{
	auto cur = goal;
	pathOut.nodes.clear();
	pathOut.cost = goal->costSoFar;

	// count path nodes to get the correct order
	uint16_t num = 0;
	while (cur)
	{
		++num;
		cur = cur->prevOnPath[taskNum];
	}

	pathOut.nodes.resize(num);
	cur = goal;
	while (cur)
	{
		pathOut.nodes[--num] = cur;
		cur = cur->prevOnPath[taskNum];
	}
}