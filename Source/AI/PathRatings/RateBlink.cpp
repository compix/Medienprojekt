#include "RateBlink.h"
#include "../../PhysixSystem.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/AIComponent.h"
#include "../../Components/BlockComponent.h"
#include "RateTrapDanger.h"

bool RateBlink::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	auto goal = path.goal();
	if (!goal->valid)
		return false;

	auto inventory = entity.component<InventoryComponent>();
	if (!inventory->isActive(SkillType::BLINK) || !AIUtil::isSafePath(entity, path))
		return false;

	bool found = false;
	float bestRating = -FLT_MAX;
	GraphNode* behaviorNode = nullptr;

	// Check in all directions if a safe line exists
	for (uint8_t i = 0; i < 4; ++i)
	{
		Direction direction = static_cast<Direction>(i);
		auto neighbor = pathEngine->getSimGraph()->getNeighbor(goal, direction);
		uint8_t numItems;
		GraphNode* endNode = nullptr;

		if (neighbor && neighbor->valid && isPromisingLine(neighbor, pathEngine->getSimGraph(), direction, path.requiredTime(entity), numItems, &endNode))
		{
			auto& personality = entity.component<AIComponent>()->personality;
			auto& desires = personality.desires;
			auto& affinity = personality.affinity;
			float rating = (numItems + (numItems > 0 ? affinity.getItem : 0.f)) * desires.getItem;
			if (goal->smell(SmellType::ENEMY) == 0 && endNode->smell(SmellType::ENEMY) > 0)
				rating += affinity.attackEnemy * desires.attackEnemy * 0.3f;

			// Consider if it's a trap
			RateTrapDanger rateTrapDanger;
			AIPath trapDangerPath;
			trapDangerPath.nodes.push_back(endNode);
			rateTrapDanger(pathEngine, trapDangerPath, entity);
			rating += trapDangerPath.rating;

			if (bestRating < rating)
			{
				bestRating = rating;
				behaviorNode = neighbor;
			}

			found = true;
		}
	}
	
	if (found)
	{
		path.rating = bestRating;
		path.behaviorNode = behaviorNode;
	}

	return found;
}

bool RateBlink::isPromisingLine(GraphNode* const startNode, Graph* graph, Direction direction, float timeForPath, uint8_t& outNumItems, GraphNode** outEndNode)
{
	assert(startNode && startNode->valid);
	auto curNode = startNode;
	GraphNode* neighbor = nullptr;

	float blinkSpeed = GameConstants::BLINK_SPEED * PhysixSystem::m_Scale;
	float timePerCell = GameConstants::CELL_WIDTH / blinkSpeed;
	outNumItems = 0;

	int i = 1;
	do
	{
		if(curNode->properties.hasItem)
			outNumItems++;

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

	*outEndNode = curNode;

	// Only consider this if there are no more blocks in the level
	return !curNode->properties.affectedByExplosion && (outNumItems > 0 || curNode->smell(SmellType::ENEMY) > 0);
}