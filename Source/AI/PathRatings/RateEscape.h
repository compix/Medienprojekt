#pragma once
#include <entityx/Entity.h>

struct AIPath;
struct GraphNode;
class PathEngine;

class RateEscape
{
public:
	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	int computeDistanceToEnemies(uint8_t x, uint8_t y);

	int computeAccumulatedDistance(AIPath& path);

	int distanceToClosest(uint8_t x, uint8_t y);
private:
	std::vector<entityx::Entity> m_entitiesToAvoid;
};