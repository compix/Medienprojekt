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

	if (!goal->valid)
		return false;

	// If the AI can't place more bombs or the path isn't safe then it fails
	auto inventory = entity.component<InventoryComponent>();
	if (inventory->getBombCount() == 0 || !AIUtil::isSafePath(entity, path))
		return false;

	// Bomb should affect blocks but no items
	bool found = true;
	float timePerCell = AIUtil::getTimePerCell(entity);
	bool spotAffectedByExplosion = goal->properties.affectedByExplosion;

	AffectedByExplosion affectedEntities;
	Bomb bomb(goal->x, goal->y, inventory->getExplosionRange(), GameConstants::EXPLOSION_TIME, inventory->activeBomb());
	pathEngine->getSimGraph()->placeBomb(bomb, &affectedEntities);
	goal->valid = false;

	uint8_t numOfAffectedBlocks = affectedEntities.numOfBlocks;
	bool blocksAffected = numOfAffectedBlocks > 0;
	bool itemsAffected = affectedEntities.numOfItems > 0;
	
	if (!blocksAffected || itemsAffected)
		found = false;

	AIPath safePath;
	AIPath fullPath;

	if (found)
	{
		// Found a potential spot -> check if a safe escape path exists
		pathEngine->searchBest(entity, goal->x, goal->y, safePath, RateSafety(), 1);
		found = safePath.nodes.size() > 1;
		fullPath.attach(path);
		fullPath.attach(safePath);
		fullPath.curNode = path.curNode;

		// If the spot is affected by an explosion, check if the path to the spot and back to safety is safe
		if (found && spotAffectedByExplosion)
		{
			if (!AIUtil::isSafePath(entity, fullPath))
				found = false;
		}
	}

	// Reset to the old state
	pathEngine->getSimGraph()->resetSimulation();

	if (found)
	{
		// Check if the path to the spot and back to safety is safe without the simulated bomb
		if (!AIUtil::isSafePath(entity, fullPath))
			found = false;
	}

	if (found)
	{
		auto& personality = entity.component<AIComponent>()->personality;
		auto& desires = personality.desires;
		auto& affinity = personality.affinity;
		path.rating = (affinity.destroyBlock + numOfAffectedBlocks - timePerCell * path.nodes.size()) * desires.destroyBlock;
	}

	return found;
}