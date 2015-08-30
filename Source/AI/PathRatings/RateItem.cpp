#include "RateItem.h"
#include "../PathFinding/PathEngine.h"
#include "../AIUtil.h"
#include "RateSafety.h"
#include "../../Components/AIComponent.h"
#include "../../Components/ItemComponent.h"
#include "../../Systems/ItemSystem.h"

RateItem::RateItem(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
}

bool RateItem::operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity)
{
	assert(m_layerManager);
	auto goal = path.goal();

	if (!goal->valid)
		return false;

	if (goal->properties.hasItem)
	{
		// Check the full path to the item and back to safety
		AIPath safePath;
		pathEngine->searchBest(entity, goal->x, goal->y, safePath, RateSafety(), 1);

		AIPath fullPath;
		fullPath.attach(path);
		fullPath.attach(safePath);

		if (AIUtil::isSafePath(entity, fullPath))
		{
			auto aiComponent = entity.component<AIComponent>();
			auto& personality = aiComponent->personality;
			auto& desires = personality.desires;
			auto& affinity = personality.affinity;
			path.rating = affinity.getItem;

			auto item = m_layerManager->getEntityWithComponent<ItemComponent>(GameConstants::MAIN_LAYER, goal->x, goal->y);
			auto itemType = item.component<ItemComponent>()->type;

			// If the AI doesn't need the item then reduce the rating (NO_NEED_FOR_ITEM_INFLUENCE is negative)
			if (!ItemSystem::needsItem(entity, itemType))
				path.rating += GameConstants::NO_NEED_FOR_ITEM_INFLUENCE;
			else
			{
				if (CommonUtil::isRare(itemType))
					path.rating += GameConstants::ITEM_RARITY_BONUS;

				if (CommonUtil::isSkill(itemType) && personality.hasFavorite(CommonUtil::toSkill(itemType)))
					path.rating += GameConstants::FAVORITE_SKILL_BONUS;
			}

			path.rating *= desires.getItem;
			path.rating -= path.requiredTime(entity);
			return true;
		}			
	}

	return false;
}