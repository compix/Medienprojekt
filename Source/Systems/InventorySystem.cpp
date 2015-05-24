#include "InventorySystem.h"
#include "../Events/BombExplodedEvent.h"
#include "../Components/OwnerComponent.h"
#include "../Components/InventoryComponent.h"

void InventorySystem::configure(entityx::EventManager& events)
{
	events.subscribe<BombExplodedEvent>(*this);
}

void InventorySystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{

}

void InventorySystem::receive(const BombExplodedEvent& e)
{
	entityx::Entity bomb = e.bomb;

	auto ownerComponent = bomb.component<OwnerComponent>();
	
	if (ownerComponent)
	{
		auto owner = ownerComponent->entity;

		if (!owner.valid())
			return;

		auto inventory = owner.component<InventoryComponent>();

		if (inventory)
		{
			inventory->bombCount++;
		}
	}
}