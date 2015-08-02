#include "SimulationGraph.h"
#include "../../Components/ItemComponent.h"
#include "../../Components/BlockComponent.h"
#include "../../Components/HealthComponent.h"
#include "../../Components/BombComponent.h"

void SimulationGraph::update(float deltaTime)
{
	for (auto x = 0; x < m_width; ++x)
	{
		for (auto y = 0; y < m_height; ++y)
		{
			m_nodeGrid[x][y] = *m_realGraph->getNode(x, y);
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

		node.properties = realNode->properties;
	}

	m_simulationPositions.clear();
}

void SimulationGraph::explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction, float futureTime)
{
	float futureExplosionTime = explosionTime + range * GameConstants::EXPLOSION_SPREAD_TIME - futureTime;

	if (futureExplosionTime <= 0.f)
		return;

	auto currentNode = getNode(x, y);
	float currentExplosionTime = currentNode->properties.timeTillExplosion;
	futureExplosionTime = explosionTime - futureTime;
	currentNode->properties.timeTillExplosion = currentNode->properties.affectedByExplosion ? std::min(futureExplosionTime, currentExplosionTime) : futureExplosionTime;
	currentNode->properties.affectedByExplosion = true;

	m_simulationPositions.push_back(SimulationPosition(x, y));

	// Simulate the explosion:
	for (int j = 0; j < range; ++j)
	{
		currentNode = getNeighbor(currentNode, direction);
		futureExplosionTime = explosionTime + (j + 1) * GameConstants::EXPLOSION_SPREAD_TIME - futureTime;
		currentExplosionTime = currentNode->properties.timeTillExplosion;
		m_simulationPositions.push_back(SimulationPosition(currentNode->x, currentNode->y));

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
			if (m_layerManager->hasEntityWithComponent<BlockComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y))
			{
				if (currentNode->properties.affectedByExplosion)
					m_nodeGrid[x][y].properties.numOfItemsAffectedByExplosion++;

				m_nodeGrid[x][y].properties.numOfBlocksAffectedByExplosion++;
				currentNode->properties.affectedByExplosion = true;

				break;
			}

			if (m_layerManager->hasEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, currentNode->x, currentNode->y))
			{
				currentNode->properties.timeTillExplosion = currentNode->properties.affectedByExplosion ? std::min(futureExplosionTime, currentExplosionTime) : futureExplosionTime;
				currentNode->properties.affectedByExplosion = true;
			}
			else
				break;
		}
	}
}