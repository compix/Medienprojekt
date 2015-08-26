#include "BombSystem.h"
#include "../Events/TimeoutEvent.h"
#include "../Components/BombComponent.h"
#include "../EntityFactory.h"
#include "../Components/CellComponent.h"
#include "../Events/EntityGotHitEvent.h"
#include "../Events/BombExplodedEvent.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"
#include "../Events/SoundEvent.h"
#include "../Components/OwnerComponent.h"

BombSystem::BombSystem()
{
}

BombSystem::~BombSystem()
{
	GameGlobals::events->unsubscribe<TimeoutEvent>(*this);
	GameGlobals::events->unsubscribe<EntityGotHitEvent>(*this);
}

void BombSystem::configure(entityx::EventManager& events)
{
	events.subscribe<TimeoutEvent>(*this);
	events.subscribe<EntityGotHitEvent>(*this);
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
	if (!entity.valid())
		return;

	if (entity.has_component<BombComponent>())
	{
		auto cell = entity.component<CellComponent>();
		auto bomb = entity.component<BombComponent>();

		if (bomb->exploded)
			return;

		GameGlobals::events->emit<SoundEvent>("explosion");
		assert(cell && bomb);

		GameGlobals::entityFactory->createExplosion(cell->x, cell->y, bomb->explosionRange, bomb->explosionSpreadTime, bomb->type);
		GameGlobals::events->emit<BombExplodedEvent>(entity);
		bomb->exploded = true;
	}
}