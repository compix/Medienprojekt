#include "GetSafe.h"
#include "../Behaviors/DoNothing.h"
#include "../PathRatings/RateSafety.h"
#include "../PathRatings/RateCombination.h"
#include "../PathRatings/RateKickBomb.h"
#include "../PathRatings/RateDesperateSaveAttempt.h"
#include "ActionType.h"
#include "../../Components/CellComponent.h"
#include "../AIUtil.h"
#include "../PathRatings/RateRiskySafety.h"

GetSafe::GetSafe(PathEngine* pathEngine, LayerManager* layerManager)
	:m_pathEngine(pathEngine)
{
	m_getSafeAction = std::make_shared<Action>(pathEngine, RateRiskySafety(), DoNothing(), layerManager);
	PathRating kickBombRating = RateCombination({ RateSafety(), RateKickBomb() });
	m_kickBombAction = std::make_shared<Action>(pathEngine, kickBombRating, DoNothing(), layerManager);
	m_tryToSurviveAction = std::make_shared<Action>(pathEngine, RateDesperateSaveAttempt(), DoNothing(), layerManager);
}

void GetSafe::preparePath(entityx::Entity& entity)
{
	auto cell = entity.component<CellComponent>();

	// No need to get safe if already safe...
	if (m_pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion)
	{
		m_getSafeAction->preparePath(entity);

		if (m_getSafeAction->path().nodes.size() > 0)
		{
			// If the rating is very low then the AI will most likely die by taking the path.
			// Instead try to survive by waiting on a cell which is most promising. Wait for a bomb to explode and maybe it will be possible to escape after.
			float timePerCell = AIUtil::getTimePerCell(entity);
			if (m_getSafeAction->getRating() < timePerCell * 0.5f)
			{
				m_currentAction = m_tryToSurviveAction.get();
				m_currentAction->preparePath(entity);
				return;
			}

			// All good so just take the path to safety
			m_currentAction = m_getSafeAction.get();
			return;
		}

		// There is no path out so try to kick a bomb to make one
		m_currentAction = m_kickBombAction.get();
		m_currentAction->preparePath(entity);
	}
}