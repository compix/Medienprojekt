#include "RateBlinkToSafety.h"
#include "../../PhysixSystem.h"
#include "../../Components/InventoryComponent.h"

bool RateBlinkToSafety::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();
	if (!goal->valid)
		return false;

	auto inventory = entity.component<InventoryComponent>();
	if (!inventory->isActive(SkillType::BLINK) || !AIUtil::isSafePath(entity, path))
		return false;

	// Check in all directions if a safe line exists
	for (uint8_t i = 0; i < 4; ++i)
	{
		Direction direction = static_cast<Direction>(i);
		auto neighbor = pathEngine->getSimGraph()->getNeighbor(goal, direction);
		if (neighbor && neighbor->valid && isSafeLine(neighbor, pathEngine->getSimGraph(), direction, path.requiredTime(entity)))
		{
			path.rating = 1.f;
			path.behaviorNode = neighbor;
			return true;
		}
	}

	return false;
}

bool RateBlinkToSafety::isSafeLine(GraphNode* const startNode, Graph* graph, Direction direction, float timeForPath)
{
	assert(startNode && startNode->valid);
	auto curNode = startNode;
	GraphNode* neighbor = nullptr;

	float blinkSpeed = GameConstants::BLINK_SPEED * PhysixSystem::m_Scale;
	float timePerCell = GameConstants::CELL_WIDTH / blinkSpeed;

	int i = 1;
	do
	{
		if (curNode->properties.affectedByExplosion)
		{
			float exploTime = curNode->properties.timeTillExplosion - timeForPath - timePerCell * i;
			if (exploTime <= timePerCell * 2.f + GameConstants::AI_UPDATE_TIME * 3.f)
				return false;
		}

		neighbor = graph->getNeighbor(curNode, direction);
		if (neighbor && neighbor->valid)
			curNode = neighbor;

		++i;

		// i < 30 because portals connected on the same line can cause a potential endless loop
		// This is not a workaround but necessary because this path might be unsafe after a certain number of iterations (through the portal)
	} while (neighbor && neighbor->valid && i < 30);

	// Only consider this if there are no more blocks in the level
	return !curNode->properties.affectedByExplosion;
}