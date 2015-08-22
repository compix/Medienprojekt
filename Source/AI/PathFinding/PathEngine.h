#pragma once
#include "Graph.h"
#include <memory>
#include "SimulationGraph.h"
#include "AIPath.h"
#include "../PathRatings/PathRating.h"

struct NodePathInfo;

enum class NodeType
{
	SAFE,
	SAFE_NOT_MARKED,
	FREE, // Cell can be entered and is not marked
	ITEM,  // item which is NOT marked
	SPOT_FOR_BOMB
};

class PathEngine;

typedef std::function<bool(const GraphNode* graphNode)> NodeCondition;

const uint8_t PATH_ENGINE_MAX_TASK_NUM = 5;

class PathEngine
{
	typedef NodePathInfo** NodePathInfoGrid;
	friend class AIVisualizer;
public:
	PathEngine(LayerManager* layerManager);
	~PathEngine();

	// Computes the path using the A* algorithm and stores it in pathOut.
	void computePath(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, AIPath& pathOut);

	// Finds the shortest path to a node of the given type
	void breadthFirstSearch(uint8_t startX, uint8_t startY, NodeCondition nodeCondition, AIPath& pathOut);

	void searchBest(entityx::Entity& entity, uint8_t startX, uint8_t startY, AIPath& pathOut, PathRating ratePath, uint8_t maxChecks = 5);

	void update(float deltaTime);

	inline Graph* getGraph() const { return m_graph.get(); }
	inline SimulationGraph* getSimGraph() const { return m_simGraph.get(); }

	void reset();
private:
	void resetPathInfo(uint8_t task);
	
	void makePath(AIPath& pathOut, NodePathInfo* goal, uint8_t taskNum);

	uint32_t estimate(const NodePathInfo& nodeInfo, const NodePathInfo& goal);

	uint8_t newTask();
	void freeTask(uint8_t task);

	inline bool inBounds(uint8_t x, uint8_t y) { return x < m_graph->m_width && y < m_graph->m_height; }
private:
	std::unique_ptr<Graph> m_graph;
	std::unique_ptr<SimulationGraph> m_simGraph;

	LayerManager* m_layerManager;

	NodePathInfoGrid m_nodeInfo[PATH_ENGINE_MAX_TASK_NUM];
	uint8_t m_freeTasks[PATH_ENGINE_MAX_TASK_NUM];
	uint8_t m_numOfFreeTasks;
};