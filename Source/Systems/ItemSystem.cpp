#include "ItemSystem.h"
#include "../Game.h"
#include "../Components/BlockComponent.h"
#include "../Components/ItemComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Events/ItemPickedUpEvent.h"
#include "../Events/BombLandedOnEntityEvent.h"
#include "../Components/ItemSpawnerComponent.h"
#include "../Components/JumpComponent.h"
#include "../Utils/Random.h"
#include "../Utils/Math.h"
#include "../Components/LavaComponent.h"

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

		if (CommonUtil::isBomb(itemType))
			inventory->remove(CommonUtil::toBomb(itemType));
		else if (CommonUtil::isSkill(itemType))
			inventory->remove(CommonUtil::toSkill(itemType));

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

		if (CommonUtil::isBomb(itemType))
			inventory->put(CommonUtil::toBomb(itemType));
		else if (CommonUtil::isSkill(itemType))
			inventory->put(CommonUtil::toSkill(itemType));

		return true;
	}

	return false;
}

void ItemSystem::configure(entityx::EventManager& events)
{
	events.subscribe<entityx::EntityDestroyedEvent>(*this);
	events.subscribe<ItemPickedUpEvent>(*this);
	events.subscribe<BombLandedOnEntityEvent>(*this);
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
			addItem(entityWithInventory, itemComponent->type);
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

	if (entity.has_component<InventoryComponent>())
	{
		auto inventory = entity.component<InventoryComponent>();

		if (inventory->items.empty())
			return;

		auto cell = entity.component<CellComponent>();
		assert(cell);

		std::vector<LevelCell> m_freeCells;
		getFreeCells(m_freeCells);
		std::shuffle(m_freeCells.begin(), m_freeCells.end(), Random::RNG());
		
		size_t freeIndex = 0;

		for (uint8_t i = 0; i < inventory->items.size() && freeIndex < m_freeCells.size(); ++i, ++freeIndex)
		{
			dropItemOnCell(LevelCell(cell->x, cell->y), m_freeCells[freeIndex], inventory->items[i]);
		}
	}
}

void ItemSystem::receive(const ItemPickedUpEvent& pickedUpEvent)
{
	auto item = pickedUpEvent.item;

	if (!item.valid())
		return;

	item.assign<DestructionComponent>(0.f);
}

void ItemSystem::receive(const BombLandedOnEntityEvent& bombLandedOnEntityEvent)
{
	auto entity = bombLandedOnEntityEvent.entity;

	if (entity && entity.has_component<InventoryComponent>())
	{
		auto inventory = entity.component<InventoryComponent>();
		auto cell = entity.component<CellComponent>();
		assert(cell);
		if (inventory->items.empty())
			return;

		int randomItem = Random::getInt(0, inventory->items.size() - 1);
		ItemType itemType = inventory->items[randomItem];

		removeItem(entity, itemType);

		// Drop item on a random free cell
		std::vector<LevelCell> m_freeCells;
		getFreeCells(m_freeCells);

		if (m_freeCells.size() > 0)
		{
			size_t random = size_t(Random::getInt(0, m_freeCells.size()));
			dropItemOnCell(LevelCell(cell->x, cell->y), m_freeCells[random], itemType);
		}
	}
}

void ItemSystem::dropItemOnCell(LevelCell from, LevelCell to, ItemType itemType)
{
	auto item = GameGlobals::entityFactory->createItem(from.x, from.y, itemType);

	Direction direction = static_cast<Direction>(Random::getInt(0, 3));
	item.assign<JumpComponent>(direction, from.x, from.y, to.x, to.y, 1, 6.f, 20.f);
}

void ItemSystem::getFreeCells(std::vector<LevelCell>& outFreeCells)
{
	for (uint8_t x = 1; x < GameGlobals::game->getWidth() - 1; ++x)
	{
		for (uint8_t y = 1; y < GameGlobals::game->getHeight() - 1; ++y)
		{
			if (m_layerManager->isFree(GameConstants::MAIN_LAYER, x, y) && !m_layerManager->hasEntityWithComponent<LavaComponent>(GameConstants::FLOOR_LAYER, x, y))
				outFreeCells.push_back(LevelCell(x, y));
		}
	}
}