#include "InventorySystem.h"
#include "../Components/OwnerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../GameGlobals.h"

InventorySystem::~InventorySystem()
{
	m_connections.removeAll();
}

void InventorySystem::addedToEngine(Engine *engine)
{
	m_connections += GameGlobals::events->bombExploded.connect(this, &InventorySystem::onBombExploded);
}

void InventorySystem::update(float dt)
{

}

void InventorySystem::onBombExploded(Entity *bomb)
{
	if (bomb->isScheduledForRemoval())
		return;

	auto ownerComponent = bomb->get<OwnerComponent>();
	
	if (ownerComponent)
	{
		auto owner = getEngine()->getEntity(ownerComponent->entityId);

		if (!owner)
			return;

		auto inventory = owner->get<InventoryComponent>();

		if (inventory)
		{
			inventory->bombCount++;
		}
	}
}