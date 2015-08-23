#include "GetSafe.h"
#include "../Behaviors/DoNothing.h"
#include "../PathRatings/RateSafety.h"
#include "../PathRatings/RateCombination.h"
#include "../PathRatings/RateKickBomb.h"
#include "../PathRatings/RateDesperateSaveAttempt.h"
#include "../../Components/CellComponent.h"
#include "../AIUtil.h"
#include "../PathRatings/RateRiskySafety.h"
#include "../PathRatings/RateTrapDanger.h"
#include "../PathRatings/RatePunchBomb.h"
#include "../Behaviors/PunchBomb.h"

bool GetSafe::done()
{
	if (ActionSelector::done())
	{
		// Wait for the bomb movement to open the path
		if (m_currentAction == m_kickBombAction.get() && m_currentAction->path().goal()->bombProperties.explosionTime > 1.5f)
			m_waitTimer = m_waitTime;
		return true;
	}

	return false;
}

GetSafe::GetSafe(PathEngine* pathEngine, LayerManager* layerManager)
	:m_pathEngine(pathEngine), m_waitTime(0.5f), m_waitTimer(0.f)
{
	m_getSafeAction = std::make_shared<Action>(pathEngine, RateCombination({ RateRiskySafety(), RateTrapDanger() }), DoNothing(), layerManager);
	m_getSafeAction->setNumOfChecks(20);
	m_kickBombAction = std::make_shared<Action>(pathEngine, RateKickBomb(), DoNothing(), layerManager);
	m_punchBomb = std::make_shared<Action>(pathEngine, RatePunchBomb(), PunchBomb(), layerManager);
	m_tryToSurviveAction = std::make_shared<Action>(pathEngine, RateDesperateSaveAttempt(), DoNothing(), layerManager);
	m_tryToSurviveAction->setNumOfChecks(20);
}

void GetSafe::preparePath(entityx::Entity& entity)
{
	auto cell = entity.component<CellComponent>();

	// No need to get safe if already safe...
	if (m_pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion)
	{
		AIPath safePath;
		NodeCondition safeSpotCondition = [](const GraphNode* node) { return !node->properties.affectedByExplosion; };
		m_pathEngine->breadthFirstSearch(cell->x, cell->y, safeSpotCondition, safePath);
		if (safePath.nodes.size() > 0)
		{
			m_getSafeAction->preparePath(entity);

			// If the risky path will certainly lead to death then try to survive by waiting on a cell which is most promising. 
			// Wait for a bomb to explode and maybe it will be possible to escape after.
			if (m_getSafeAction->path().nodes.size() == 0)
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

		if (m_currentAction->path().nodes.size() == 0)
		{
			// Couldn't kick the bomb, try to punch it
			m_currentAction = m_punchBomb.get();
			m_currentAction->preparePath(entity);
		}

		return;
	}

	m_currentAction = nullptr;
}

void GetSafe::update(entityx::Entity& entity, float deltaTime)
{
	m_waitTimer -= deltaTime;
	if (m_waitTimer <= 0.f)
		m_currentAction->update(entity, deltaTime);
}