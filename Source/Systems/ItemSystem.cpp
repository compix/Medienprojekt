#include "ItemSystem.h"
#include "../Game.h"
#include "../Components/BlockComponent.h"
#include "../Utils/Random.h"
#include "../Components/ItemComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Events/ItemPickedUpEvent.h"

ItemSystem::ItemSystem(LayerManager* layerManager)
	: m_layerManager(layerManager)
{
}

ItemSystem::~ItemSystem()
{
	GameGlobals::events->unsubscribe<entityx::EntityDestroyedEvent>(*this);
	GameGlobals::events->unsubscribe<ItemPickedUpEvent>(*this);
}

void ItemSystem::configure(entityx::EventManager& events)
{
	events.subscribe<entityx::EntityDestroyedEvent>(*this);
	events.subscribe<ItemPickedUpEvent>(*this);
}

void ItemSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto item : entityManager.entities_with_components<ItemComponent, CellComponent, LayerComponent>())
	{
		if (!item.valid() || item.has_component<DestructionComponent>())
			continue;

		auto cellComponent = item.component<CellComponent>();
		auto layerComponent = item.component<LayerComponent>();
		auto itemComponent = item.component<ItemComponent>();

		auto entityWithInventory = m_layerManager->getEntityWithComponent<InventoryComponent>(layerComponent->layer, cellComponent->x, cellComponent->y);
		if (entityWithInventory.valid())
		{
			auto inventory = entityWithInventory.component<InventoryComponent>();

			switch (itemComponent->type)
			{
			case ItemType::BOMB_CAP_BOOST:
				if (inventory->bombCount < GameConstants::BOMB_CAP)
					++inventory->bombCount;
				break;
			default: break;
			}

			eventManager.emit<ItemPickedUpEvent>(item, entityWithInventory);
		}
	}
}

void ItemSystem::receive(const entityx::EntityDestroyedEvent& e)
{
	if (!e.entity.valid())
		return;

	if (e.entity.has_component<BlockComponent>())
	{
		auto entity = e.entity;

		auto cell = entity.component<CellComponent>();
		assert(cell);

		if (Random::getInt(1, 100) <= 50) // 50% Chance to spawn an item
		{
			// TODO: Create random items taking rarity and minimum spawn number into consideration, 
			// assigning ItemComponents with blocks during Level creation might be a good idea
			GameGlobals::entityFactory->createItem(cell->x, cell->y, ItemType::BOMB_CAP_BOOST);
		} 
		else if (Random::getInt(1, 100) <= 50)
		{
			GameGlobals::entityFactory->createItem(cell->x, cell->y, ItemType::BOMB_KICK_SKILL);
		}
	}
}

void ItemSystem::receive(const ItemPickedUpEvent& e)
{
	auto item = e.item;

	if (!item.valid())
		return;

	item.assign<DestructionComponent>(0.f);
}