#include "BombSystem.h"

#include "../Components/BombComponent.h"
#include "../EntityFactory.h"
#include "../Components/CellComponent.h"
#include "../Components/DestructionComponent.h"

#include "../Components/TimerComponent.h"

#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"


BombSystem::BombSystem()
{
}

BombSystem::~BombSystem()
{
	GameGlobals::events->unsubscribe<TimeoutEvent>(*this);
	GameGlobals::events->unsubscribe<EntityGotHitEvent>(*this);
}

void BombSystem::addedToEngine(Engine *engine)
{
	events.subscribe<TimeoutEvent>(*this);
	events.subscribe<EntityGotHitEvent>(*this);
}

void BombSystem::update(float dt)
{
}

void BombSystem::onTimeout(Entity *affectedEntity)
{
	detonate(affectedEntity);
}

void BombSystem::onEntityGotHit(Entity *damageDealer, Entity *damagedEntity, int damage)
{
	detonate(damagedEntity);
}

void BombSystem::detonate(Entity *entity)
{
	if (!entity->isValid())
		return;

	if (entity->has<BombComponent>())
	{
		auto cell = entity->get<CellComponent>();
		auto bomb = entity->get<BombComponent>();

		if (bomb->exploded)
			return;

		GameGlobals::events->sound.emit("explosion");
		assert(cell);

		GameGlobals::entityFactory->createExplosion(cell->x, cell->y, bomb->explosionRange, bomb->explosionSpreadTime);
		GameGlobals::events->bombExploded.emit(entity);
		bomb->exploded = true;
	}
}