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

	//void placeBomb(uint8_t x, uint8_t y, uint8_t range, float explosionTime, float futureTime) override;

	void resetSimulation();

protected:
	void explosionSpread(uint8_t x, uint8_t y, uint8_t range, float explosionTime, Direction direction, float futureTime = 0.0f) override;

private:
	Graph* m_realGraph;

	std::vector<SimulationPosition> m_simulationPositions;
};