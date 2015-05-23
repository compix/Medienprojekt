#include "BombSystem.h"
#include <Events/TimeoutEvent.h>
#include <Components/BombComponent.h>
#include "EntityFactory.h"
#include <Components/CellComponent.h>
#include <Components/DestructionComponent.h>
#include <Events/EntityGotHitEvent.h>
#include <Components/TimerComponent.h>
#include <Events/BombExplodedEvent.h>

BombSystem::BombSystem(EntityFactory* entityFactory)
	:m_entityFactory(entityFactory)
{
}

void BombSystem::configure(entityx::EventManager& events)
{
	events.subscribe<TimeoutEvent>(*this);
	events.subscribe<EntityGotHitEvent>(*this);

	m_eventManager = &events;
}

void BombSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
}

void BombSystem::receive(const TimeoutEvent& timeoutEvent)
{
	detonate(timeoutEvent.affectedEntity);
}

void BombSystem::receive(const EntityGotHitEvent& entityGotHitEvent)
{
	detonate(entityGotHitEvent.damagedEntity);
}

void BombSystem::detonate(entityx::Entity entity)
{
	if (!entity.valid() || entity.has_component<DestructionComponent>())
		return;

	if (entity.has_component<BombComponent>())
	{
		auto cell = entity.component<CellComponent>();
		auto bomb = entity.component<BombComponent>();

		assert(cell);

		m_entityFactory->createExplosion(cell->y, cell->x, bomb->explosionRange, bomb->explosionSpreadTime);
		if (!entity.has_component<DestructionComponent>())
			entity.assign<DestructionComponent>();

		m_eventManager->emit<BombExplodedEvent>(entity);
	}
}