#include "RateDestroyBlockSpot.h"
#include "../../GameConstants.h"
#include "RateSafety.h"
#include "../PathFinding/PathEngine.h"
#include "../AIUtil.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/AIComponent.h"

bool RateDestroyBlockSpot::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();

	if (!goal)
		return false;

	// If the AI can't place more bombs or the path isn't safe then it fails
	auto inventory = entity.component<InventoryComponent>();
	if (inventory->bombCount == 0 || !AIUtil::isSafePath(entity, path))
		return false;

	// Bomb should affect blocks but no items
	bool found = true;
	float timePerCell = AIUtil::getTimePerCell(entity);
	bool spotAffectedByExplosion = goal->properties.affectedByExplosion;

	AffectedByExplosion affectedEntities;
	pathEngine->getSimGraph()->placeBomb(goal->x, goal->y, inventory->explosionRange, GameConstants::EXPLOSION_TIME, &affectedEntities);

	uint8_t numOfAffectedBlocks = affectedEntities.numOfBlocks;
	bool blocksAffected = numOfAffectedBlocks > 0;
	bool itemsAffected = affectedEntities.numOfItems > 0;
	
	if (!blocksAffected || itemsAffected)
		found = false;

	if (found)
	{
		// Found a potential spot -> check if a safe escape path exists
		AIPath safePath;
		pathEngine->searchBest(entity, goal->x, goal->y, safePath, RateSafety(), 1);

		if (safePath.nodes.size() < 2 || !AIUtil::isSafePath(entity, safePath))
			found = false;

		if (found && spotAffectedByExplosion)
		{
			// Check the full path (to bomb and back to safety) to be sure if it's really safe
			AIPath fullPath;
			fullPath.attach(path);
			fullPath.attach(safePath);
			fullPath.curNode = path.curNode;

			if (!AIUtil::isSafePath(entity, fullPath))
				found = false;
		}
	}

	if (found)
	{
		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = (affinity.destroyBlock + numOfAffectedBlocks - timePerCell * path.nodes.size()) * desires.destroyBlock;
	}

	// Reset to the old state
	pathEngine->getSimGraph()->resetSimulation();

	return found;
}