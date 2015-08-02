#pragma once
#include <cinttypes>
#include <entityx/Entity.h>

struct Path;
struct GraphNode;
class PathEngine;

class RateEscape
{
public:
	explicit RateEscape(entityx::Entity& entity, std::vector<entityx::Entity>& entitiesToAvoid);
	bool operator()(PathEngine* pathEngine, GraphNode* node, Path& pathOut, uint8_t taskNum);

private:
	int computeDistanceToEnemies(uint8_t x, uint8_t y);

	int computeAccumulatedDistance(Path& path);

	int distanceToClosest(uint8_t x, uint8_t y);
private:
	std::vector<entityx::Entity> m_entitiesToAvoid;
	entityx::Entity m_entity;
	int m_startDistance;
};