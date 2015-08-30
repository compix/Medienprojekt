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
	:m_throwStarted(false), m_pathEngine(pathEngine)
{
	m_currentAction = nullptr;
	m_placeBomb = std::make_shared<Action>(pathEngine, RateCombination({ RateAttackEnemy(), RateTrapDanger() }), PlaceBomb(), layerManager);
	m_throwBomb = std::make_shared<Action>(pathEngine, RateCombination({ RateThrowBomb(), RateTrapDanger() }), ThrowBomb(layerManager), layerManager);

	// Activate only if the AI can place bombs.
	setActivationCondition([&](entityx::Entity& entity, float deltaTime)
	{
		auto inventory = entity.component<InventoryComponent>();
		return inventory->getAvailableBombCount() > 0;
	});
}

void AttackEnemy::preparePath(entityx::Entity& entity)
{
	auto inventory = entity.component<InventoryComponent>();
	int random = Random::getInt(1, 100);

	// 5% chance to throw a bomb
	if (inventory->canHoldBomb() && (random <= 5 || inventory->isHoldingBomb))
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
		// The action fails if the enemy didn't place the bomb yet and has no bombs to place
		if (!m_placedBomb && inventory->getAvailableBombCount() == 0)
		{
			m_currentAction = nullptr;
			m_throwStarted = false;
			return;
		}

		// First place the bomb then pick it up
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