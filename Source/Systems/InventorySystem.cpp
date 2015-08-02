#include "InventorySystem.h"

#include "../Components/OwnerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../GameGlobals.h"



InventorySystem::~InventorySystem()
{
	GameGlobals::events->unsubscribe<BombExplodedEvent>(*this);
}

void InventorySystem::addedToEngine(Engine *engine)
{
	events.subscribe<BombExplodedEvent>(*this);
}

void InventorySystem::update(float dt)
{

}

void InventorySystem::onBombExploded(Entity *bomb)
{
	if (!bomb->isValid())
		return;

	auto ownerComponent = bomb->get<OwnerComponent>();
	
	if (ownerComponent)
	{
		auto owner = ownerComponent->entity;

		if (!owner->isValid())
			return;

		auto inventory = owner->get<InventoryComponent>();

		if (inventory)
		{
			inventory->bombCount++;
		}
	}
}