#include "AttackEnemy.h"
#include "../PathRatings/RateTrapDanger.h"
#include "../PathRatings/RateEscape.h"
#include "../PathRatings/RateAttackEnemy.h"
#include "../PathRatings/RateCombination.h"
#include "../Behaviors/PlaceBomb.h"
#include "../PathRatings/RateThrowBomb.h"
#include "../Behaviors/ThrowBomb.h"
#include "../../Utils/Random.h"

AttackEnemy::AttackEnemy(PathEngine* pathEngine, LayerManager* layerManager)
	:m_throwStarted(false)
{
	m_currentAction = nullptr;
	m_placeBomb = std::make_shared<Action>(pathEngine, RateCombination({ RateAttackEnemy(), RateEscape(), RateTrapDanger() }), PlaceBomb(), layerManager);
	m_throwBomb = std::make_shared<Action>(pathEngine, RateCombination({ RateThrowBomb(), RateEscape(), RateTrapDanger() }), ThrowBomb(layerManager), layerManager);
}

bool AttackEnemy::valid(entityx::Entity& entity)
{
	return m_currentAction &&
		m_currentAction->path().nodes.size() > 0 &&
		!AIUtil::isBlocked(m_currentAction->path()) &&
		AIUtil::isValidPath(m_currentAction->path()) &&
		AIUtil::isOnPath(entity, m_currentAction->path()) &&
		AIUtil::isSafePath(entity, m_currentAction->path());
}

void AttackEnemy::preparePath(entityx::Entity& entity)
{
	auto inventory = entity.component<InventoryComponent>();
	int random = Random::getInt(0, 4);

	if (random == 0 || inventory->isHoldingBomb)
	{
		m_currentAction = m_throwBomb.get();
		m_currentAction->preparePath(entity);
		if (!m_currentAction->path().nodes.empty())
		{
			m_throwStarted = true;
			return;
		}
		
		if (inventory->isHoldingBomb)
			return;
	}

	m_currentAction = m_placeBomb.get();
	m_currentAction->preparePath(entity);
}

void AttackEnemy::update(entityx::Entity& entity, float deltaTime)
{
	auto inventory = entity.component<InventoryComponent>();
	if (inventory->isHoldingBomb)
		m_throwStarted = false;

	if (m_throwStarted)
	{
		if (!m_placedBomb && inventory->getAvailableBombCount() == 0)
		{
			m_currentAction = nullptr;
			m_throwStarted = false;
			return;
		}

		// Picking up the bomb is the same input so placeBomb behavior will work too
		PlaceBomb placeBomb;
		placeBomb(entity);
		m_placedBomb = true;
		return;
	}

	m_placedBomb = false;
	m_throwStarted = false;

	if (m_currentAction)
		m_currentAction->update(entity, deltaTime);
}

bool AttackEnemy::done()
{
	return m_currentAction && m_currentAction->done();
}