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
	m_maxItemCounts[ItemType::BOMB_CAP_BOOST] = 8;
	m_maxItemCounts[ItemType::BOMB_KICK_SKILL] = 1;
	m_maxItemCounts[ItemType::SPEED_MULTIPLICATOR] = 6;
	m_maxItemCounts[ItemType::BOMB_RANGE_BOOST] = 10;
	m_maxItemCounts[ItemType::PORTAL_SKILL] = 1;
	m_maxItemCounts[ItemType::ANTI_MAGNET_SKILL] = 1;
	m_maxItemCounts[ItemType::PUNCH_SKILL] = 1;
	m_maxItemCounts[ItemType::HOLD_BOMB_SKILL] = 1;
	m_maxItemCounts[ItemType::BLINK_SKILL] = 1;
	m_maxItemCounts[ItemType::GHOST_BOMB] = 1;
	m_maxItemCounts[ItemType::LIGHTNING_BOMB] = 1;

	m_minItemCounts[ItemType::BOMB_CAP_BOOST] = 1;
	m_minItemCounts[ItemType::BOMB_KICK_SKILL] = 0;
	m_minItemCounts[ItemType::SPEED_MULTIPLICATOR] = 0;
	m_minItemCounts[ItemType::BOMB_RANGE_BOOST] = 2;
	m_minItemCounts[ItemType::PORTAL_SKILL] = 0;
	m_minItemCounts[ItemType::ANTI_MAGNET_SKILL] = 0;
	m_minItemCounts[ItemType::PUNCH_SKILL] = 0;
	m_minItemCounts[ItemType::HOLD_BOMB_SKILL] = 0;
	m_minItemCounts[ItemType::BLINK_SKILL] = 0;
	m_minItemCounts[ItemType::GHOST_BOMB] = 0;
	m_minItemCounts[ItemType::LIGHTNING_BOMB] = 0;
}

ItemSystem::~ItemSystem()
{
	GameGlobals::events->unsubscribe<entityx::EntityDestroyedEvent>(*this);
	GameGlobals::events->unsubscribe<ItemPickedUpEvent>(*this);
}

bool ItemSystem::removeItem(entityx::Entity& entity, ItemType itemType)
{
	auto inventory = entity.component<InventoryComponent>();

	if (inventory->itemCounts[itemType] > m_minItemCounts[itemType])
	{
		inventory->itemCounts[itemType]--;
		auto& items = inventory->items;
		items.erase(std::remove(items.begin(), items.end(), itemType), items.end());
		return true;
	}

	return false;
}

bool ItemSystem::addItem(entityx::Entity& entity, ItemType itemType)
{
	auto inventory = entity.component<InventoryComponent>();

	if (inventory->itemCounts[itemType] < m_maxItemCounts[itemType])
	{
		inventory->itemCounts[itemType]++;
		auto& items = inventory->items;
		items.push_back(itemType);
		return true;
	}

	return false;
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
			ItemType itemType = itemComponent->type;

			addItem(entityWithInventory, itemType);

			if (CommonUtil::isBomb(itemType))
				inventory->put(CommonUtil::toBomb(itemType));
			else if (CommonUtil::isSkill(itemType))
				inventory->put(CommonUtil::toSkill(itemType));

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