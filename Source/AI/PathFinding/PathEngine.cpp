#include "PathEngine.h"
#include <queue>
#include "../../Game.h"
#include "AIPath.h"
#include "NodePathInfo.h"
#include "../../Components/AIComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Utils/Common.h"
#include "../../Components/CellComponent.h"
#include "../../Components/LayerComponent.h"
#include "../../Utils/Random.h"

PathEngine::PathEngine(LayerManager* layerManager)
	:m_layerManager(layerManager), m_numOfFreeTasks(PATH_ENGINE_MAX_TASK_NUM)
{
	m_graph = std::make_unique<Graph>(layerManager);
	m_simGraph = std::make_unique<SimulationGraph>(layerManager, m_graph.get());

	for (uint8_t i = 0; i < PATH_ENGINE_MAX_TASK_NUM; ++i)
	{
		m_nodeInfo[i] = new NodePathInfo*[m_graph->m_width];
		for (uint8_t j = 0; j < m_graph->m_width; ++j)
			m_nodeInfo[i][j] = new NodePathInfo[m_graph->m_height];
	}

	for (uint8_t i = 0; i < PATH_ENGINE_MAX_TASK_NUM; ++i)
	{
		for (uint8_t x = 0; x < m_graph->m_width; ++x)
		{
			for (uint8_t y = 0; y < m_graph->m_height; ++y)
			{
				m_nodeInfo[i][x][y].x = x;
				m_nodeInfo[i][x][y].y = y;
			}
		}
	}

	for (uint8_t i = 0; i < PATH_ENGINE_MAX_TASK_NUM; ++i)
		m_freeTasks[i] = PATH_ENGINE_MAX_TASK_NUM - i - 1;
}

PathEngine::~PathEngine()
{
	for (uint8_t i = 0; i < PATH_ENGINE_MAX_TASK_NUM; ++i)
	{
		for (uint8_t j = 0; j < m_graph->m_width; ++j)
			delete[] m_nodeInfo[i][j];

		delete m_nodeInfo[i];
	}
}

void PathEngine::computePath(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, AIPath& pathOut)
{
	assert(inBounds(startX, startY) && inBounds(endX, endY));
	uint8_t taskNum = newTask();
	NodePathInfo& goal = m_nodeInfo[taskNum][endX][endY];

	NodePathInfo& start = m_nodeInfo[taskNum][startX][startY];
	start.costSoFar = 0;
	start.estimatedTotalCost = estimate(start, goal);
	start.prevOnPath = nullptr;

	resetPathInfo(taskNum);

	NodePathInfo* min = &start;
	min->next = nullptr;
	min->prev = nullptr;

	while (min)
	{
		// close this node
		min->state = NodeState::CLOSED;

		if (min == &goal)
			break;

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			auto neighborNode = m_graph->getNeighbor(m_graph->getNode(min->x, min->y), static_cast<Direction>(i));

			if (!neighborNode || !neighborNode->valid)
				continue;

			NodePathInfo& neighbor = m_nodeInfo[taskNum][neighborNode->x][neighborNode->y];
			switch (neighbor.state)
			{
			case NodeState::UNVISITED:
				// New node - Initialize it.
				neighbor.costSoFar = min->costSoFar + neighbor.cost;
				neighbor.estimatedTotalCost = neighbor.costSoFar + estimate(neighbor, goal);
				neighbor.prevOnPath = min;
				neighbor.state = NodeState::OPEN;
				neighbor.insert(min);
				break;
			case NodeState::OPEN:
				// update/relax the node if necessary
				if (min->costSoFar + neighbor.cost + estimate(neighbor, goal) < neighbor.estimatedTotalCost)
				{
					neighbor.costSoFar = min->costSoFar + neighbor.cost;
					neighbor.estimatedTotalCost = neighbor.costSoFar + estimate(neighbor, goal);
					neighbor.prevOnPath = min;
					// update list position
					NodePathInfo* at = neighbor.remove();
					assert(at); // "at" can't possibly be a nullptr
					neighbor.insert(at);
				}
				break;
			case NodeState::CLOSED: break;
			default: break;
			}
		}
		
		min = min->remove();

		// The estimation can be inaccurat if portals are present so it is possible that a lower min exists now
		while (min && min->prev) min = min->prev;
	}

	if (min)
		makePath(pathOut, min, taskNum);

	// Done
	freeTask(taskNum);
}

void PathEngine::breadthFirstSearch(uint8_t startX, uint8_t startY, NodeCondition nodeCondition, AIPath& pathOut)
{
	assert(inBounds(startX, startY));
	uint8_t taskNum = newTask();
	resetPathInfo(taskNum);

	std::queue<NodePathInfo*> processQueue;
	NodePathInfo& start = m_nodeInfo[taskNum][startX][startY];
	start.prevOnPath = nullptr;
	processQueue.push(&start);
	start.state = NodeState::CLOSED;

	while (processQueue.size() > 0)
	{
		NodePathInfo* curInfo = processQueue.front();
		GraphNode* curNode = m_simGraph->getNode(curInfo->x, curInfo->y);
		processQueue.pop();

		if (nodeCondition(curNode))
		{
			makePath(pathOut, curInfo, taskNum);
			// Done
			freeTask(taskNum);
			return;
		}

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighborNode = m_simGraph->getNeighbor(curNode, static_cast<Direction>(i));

			if (!neighborNode || !neighborNode->valid)
				continue;
			
			NodePathInfo& neighbor = m_nodeInfo[taskNum][neighborNode->x][neighborNode->y];
			if (neighbor.state == NodeState::UNVISITED)
			{
				neighbor.prevOnPath = curInfo;
				neighbor.state = NodeState::CLOSED;
				processQueue.push(&neighbor);
			}
		}
	}

	pathOut.nodes.clear();
	pathOut.rating = -FLT_MAX;
	
	// Done
	freeTask(taskNum);
}

void PathEngine::searchBest(entityx::Entity& entity, uint8_t startX, uint8_t startY, AIPath& pathOut, PathRating ratePath, uint8_t maxChecks)
{
	std::vector<AIPath> paths;
	search(entity, startX, startY, ratePath, paths, maxChecks);

	if (!paths.empty())
	{
		uint32_t bestIndex = 0;
		for (uint32_t i = 1; i < paths.size(); ++i)
			if (paths[bestIndex].rating < paths[i].rating)
				bestIndex = i;
		pathOut = paths[bestIndex];
		return;
	}

	pathOut = AIPath();
}

void PathEngine::search(entityx::Entity& entity, uint8_t startX, uint8_t startY, PathRating ratePath, std::vector<AIPath>& outPaths, uint8_t maxPaths)
{
	// There is no such thing as "assert overuse"
	assert(maxPaths > 0);
	assert(entity);
	assert(entity.has_component<AIComponent>() && entity.has_component<InventoryComponent>());
	assert(entity.has_component<TransformComponent>() && entity.has_component<CellComponent>());
	assert(entity.has_component<LayerComponent>());
	assert(inBounds(startX, startY));

	uint8_t taskNum = newTask();
	resetPathInfo(taskNum);

	std::queue<NodePathInfo*> processQueue;
	NodePathInfo& start = m_nodeInfo[taskNum][startX][startY];
	start.prevOnPath = nullptr;
	processQueue.push(&start);
	start.state = NodeState::CLOSED;
	AIPath path;

	while (processQueue.size() > 0)
	{
		NodePathInfo* curInfo = processQueue.front();
		processQueue.pop();

		makePath(path, curInfo, taskNum);
		if (ratePath(this, path, entity))
		{
			maxPaths--;
			outPaths.push_back(path);
		}

		if (maxPaths == 0)
			break;

		GraphNode* curNode = m_simGraph->getNode(curInfo->x, curInfo->y);

		// Don't go through neighbors of invalid nodes (except for the first node)
		if (curInfo != &start && !curNode->valid)
			continue;

		// Go through all neighbors
		for (uint8_t i = 0; i < 4; ++i)
		{
			GraphNode* neighborNode = m_simGraph->getNeighbor(curNode, static_cast<Direction>(i));
			NodePathInfo& neighbor = m_nodeInfo[taskNum][neighborNode->x][neighborNode->y];
			if (!neighborNode || (!neighborNode->properties.hasBomb && !neighborNode->valid))
				continue;

			if (neighbor.state == NodeState::UNVISITED)
			{
				neighbor.prevOnPath = curInfo;
				neighbor.state = NodeState::CLOSED;
				processQueue.push(&neighbor);
			}
		}
	}

	// Done
	freeTask(taskNum);
}

void PathEngine::searchRandom(entityx::Entity& entity, uint8_t startX, uint8_t startY, AIPath& pathOut, PathRating ratePath, uint8_t outOf)
{
	std::vector<AIPath> paths;
	search(entity, startX, startY, ratePath, paths, outOf);
	pathOut = paths.empty() ? AIPath() : paths[Random::getInt(0, paths.size() - 1)];
}

void PathEngine::update(float deltaTime)
{
	m_graph->update(deltaTime);
	m_simGraph->update(deltaTime);
}

uint32_t PathEngine::estimate(const NodePathInfo& nodeInfo, const NodePathInfo& goal)
{
	return abs(nodeInfo.x - goal.x) + abs(nodeInfo.y - goal.y);
}

uint8_t PathEngine::newTask()
{
	assert(m_numOfFreeTasks > 0); 
	return m_freeTasks[--m_numOfFreeTasks];
}

void PathEngine::freeTask(uint8_t task)
{
	assert(m_numOfFreeTasks < PATH_ENGINE_MAX_TASK_NUM); 
	m_freeTasks[m_numOfFreeTasks] = task;
	m_numOfFreeTasks++;
}

void PathEngine::reset()
{
	m_simGraph->reset();
	m_graph->reset();
}

void PathEngine::resetPathInfo(uint8_t task)
{
	for (uint8_t x = 0; x < m_graph->m_width; ++x)
	{
		for (uint8_t y = 0; y < m_graph->m_height; ++y)
		{
			m_nodeInfo[task][x][y].state = NodeState::UNVISITED;
		}
	}
}

void PathEngine::makePath(AIPath& pathOut, NodePathInfo* goal, uint8_t task)
{
	auto cur = goal;
	pathOut.nodes.clear();
	pathOut.cost = goal->costSoFar;

	// count path nodes to get the correct order
	uint16_t num = 0;
	while (cur)
	{
		++num;
		cur = cur->prevOnPath;
	}

	pathOut.nodes.resize(num);
	cur = goal;
	while (cur)
	{
		pathOut.nodes[--num] = m_simGraph->getNode(cur->x, cur->y);
		cur = cur->prevOnPath;
	}
}
