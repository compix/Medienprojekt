#pragma once
#include "Graph.h"

struct SimulationPosition
{
	SimulationPosition(uint8_t x, uint8_t y) : x(x), y(y) {}

	uint8_t x, y;
};

class SimulationGraph : public Graph
{
public:
	void update(float deltaTime) override;
public:
	explicit SimulationGraph(LayerManager* layerManager, Graph* realGraph);

	void resetSimulation();

protected:
	void setOnFire(uint8_t x, uint8_t y, float explosionTime) override;
private:
	Graph* m_realGraph;

	std::vector<SimulationPosition> m_simulationPositions;
};