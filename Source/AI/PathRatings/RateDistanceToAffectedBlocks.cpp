#include "RateDistanceToAffectedBlocks.h"
#include "../../Components/CellComponent.h"
#include "../../Game.h"
#include "../../Components/AIComponent.h"

bool RateDistanceToAffectedBlocks::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	if (!path.goal() || path.goal()->smells.dyingBlock == 0)
		return false;

	auto& personality = entity.component<AIComponent>()->personality;
	auto& desires = personality.desires;
	auto& affinity = personality.affinity;
	path.rating = affinity.wait + path.goal()->smells.dyingBlock;
	path.rating *= desires.wait;
	return true;
}