#include "SimulationGraph.h"

void SimulationGraph::update(float deltaTime)
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y] = *m_realGraph->getNode(x, y);

			// Set other Portal to the correct pointer
			auto otherPortal = m_realGraph->getNode(x, y)->properties.otherPortal;
			if (otherPortal)
				m_nodeGrid[x][y].properties.otherPortal = &m_nodeGrid[otherPortal->x][otherPortal->y];
		}
	}
}

SimulationGraph::SimulationGraph(LayerManager* layerManager, Graph* realGraph)
	: Graph(layerManager), m_realGraph(realGraph)
{
}

void SimulationGraph::resetSimulation()
{
	for (auto& pos : m_simulationPositions)
	{
		auto realNode = m_realGraph->getNode(pos.x, pos.y);
		auto& node = m_nodeGrid[pos.x][pos.y];

		bool wasMarked = node.marked;
		node = *realNode;
		node.marked = wasMarked;
		// Set other Portal to the correct pointer
		auto otherPortal = m_realGraph->getNode(pos.x, pos.y)->properties.otherPortal;
		if (otherPortal)
			m_nodeGrid[pos.x][pos.y].properties.otherPortal = &m_nodeGrid[otherPortal->x][otherPortal->y];
	}

	m_simulationPositions.clear();
}

void SimulationGraph::setOnFire(uint8_t x, uint8_t y, float explosionTime)
{
	Graph::setOnFire(x, y, explosionTime);
	m_simulationPositions.push_back(SimulationPosition(x, y));
	auto otherPortal = m_nodeGrid[x][y].properties.otherPortal;
	if (otherPortal)
		m_simulationPositions.push_back(SimulationPosition(otherPortal->x, otherPortal->y));
}