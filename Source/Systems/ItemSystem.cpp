#include "ItemSystem.h"
#include "../Game.h"
#include "../Components/BlockComponent.h"
#include "../Components/ItemComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Events/ItemPickedUpEvent.h"
#include "../Components/ItemSpawnerComponent.h"
#include "../Components/JumpComponent.h"

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
		if (entityWithInventory.valid() && !item.has_component<JumpComponent>())
		{
			auto inventory = entityWithInventory.component<InventoryComponent>();

			switch (itemComponent->type)
			{
			case ItemType::BOMB_CAP_BOOST:
				if (inventory->bombCount < GameConstants::BOMB_CAP)
					++inventory->bombCount;
				break;
			case ItemType::BOMB_KICK_SKILL:
				if (inventory->bombKick == false)
					inventory->bombKick = true;
				break;
			case ItemType::SPEED_MULTIPLICATOR:
				if (inventory->speedMultiplicator < GameConstants::SPEED_MULTI_CAP)
					inventory->speedMultiplicator += GameConstants::SPEED_MULTI_INC;
				break;
			case ItemType::BOMB_RANGE_BOOST:
				if (inventory->explosionRange < GameConstants::BOMB_RANGE_CAP)
					++inventory->explosionRange;
				break;
			case ItemType::PORTAL_SKILL:
				inventory->put(SkillType::PLACE_PORTAL);
				break;
			case ItemType::ANTI_MAGNET_SKILL:
				if (inventory->antiMagnet == false)
					inventory->antiMagnet = true;
				break;
			case ItemType::PUNCH_SKILL:
				inventory->put(SkillType::PUNCH);
				break;
			case ItemType::BLINK_SKILL:
				inventory->put(SkillType::BLINK);
				break;
			case ItemType::HOLD_BOMB_SKILL:
				if (inventory->canHold == false)
					inventory->canHold = true;
				break;
			case ItemType::GHOST_BOMB:
				inventory->put(BombType::GHOST);
				break;
			case ItemType::LIGHTNING_BOMB:
				inventory->put(BombType::LIGHTNING);
				break;
			default: break;
			}

			eventManager.emit<ItemPickedUpEvent>(item, entityWithInventory);
		}
	}
}

void ItemSystem::receive(const entityx::EntityDestroyedEvent& destroyedEvent)
{
	auto entity = destroyedEvent.entity;

	if (!entity)
		return;

	if (entity.has_component<BlockComponent>() && entity.has_component<ItemSpawnerComponent>())
	{
		auto cell = entity.component<CellComponent>();
		auto itemSpawn = entity.component<ItemSpawnerComponent>();
		assert(cell);

		GameGlobals::entityFactory->createItem(cell->x, cell->y, itemSpawn->itemType);
	}
}

void ItemSystem::receive(const ItemPickedUpEvent& pickedUpEvent)
{
	auto item = pickedUpEvent.item;

	if (!item.valid())
		return;

	item.assign<DestructionComponent>(0.f);
}