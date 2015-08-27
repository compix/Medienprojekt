#include "InventorySystem.h"
#include "../Events/BombExplodedEvent.h"
#include "../Components/OwnerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../GameGlobals.h"
#include "../Events/TimeoutEvent.h"


InventorySystem::~InventorySystem()
{
	GameGlobals::events->unsubscribe<BombExplodedEvent>(*this);
}

void InventorySystem::configure(entityx::EventManager& events)
{
	events.subscribe<BombExplodedEvent>(*this);
}

void InventorySystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{

}

void InventorySystem::receive(const BombExplodedEvent& e)
{
	auto bomb = e.bomb;

	if (!bomb.valid())
		return;

	auto ownerComponent = bomb.component<OwnerComponent>();
	
	if (ownerComponent)
	{
		auto owner = ownerComponent->entity;

		if (!owner.valid())
			return;

		auto inventory = owner.component<InventoryComponent>();

		if (inventory)
		{
			inventory->itemCounts[ItemType::BOMB_CAP_BOOST]++;
		}
	}
}