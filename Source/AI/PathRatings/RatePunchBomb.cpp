#include "RatePunchBomb.h"
#include "../../Components/InventoryComponent.h"
#include "../AIUtil.h"
#include "../../Components/AIComponent.h"

bool RatePunchBomb::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	auto inventory = entity.component<InventoryComponent>();
	assert(inventory);
	if (!inventory->isActive(SkillType::PUNCH))
		return false;

	auto graph = pathEngine->getSimGraph();
	GraphNode* bestBomb = nullptr; // Best = highest explosion time -> usually safest

	// Check if there is a bomb on the neighbor cells
	for (uint8_t i = 0; i < 4; ++i)
	{
		Direction neighborDirection = static_cast<Direction>(i);
		auto neighbor = graph->getNeighbor(goal, neighborDirection);
		if (!neighbor || !neighbor->properties.hasBomb)
			continue;

		if (!bestBomb || neighbor->bombProperties.explosionTime > bestBomb->bombProperties.explosionTime)
			bestBomb = neighbor;
	}

	if (bestBomb)
	{
		path.rating = bestBomb->bombProperties.explosionTime - path.nodes.size() * AIUtil::getTimePerCell(entity);
		auto aiComponent = entity.component<AIComponent>();
		assert(aiComponent);
		aiComponent->behaviorNode = bestBomb;
		return true;
	}

	return false;
}