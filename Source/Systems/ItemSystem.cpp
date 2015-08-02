#include "ItemSystem.h"
#include "../Game.h"
#include "../Components/BlockComponent.h"
#include "../Utils/Random.h"
#include "../Components/ItemComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/DestructionComponent.h"


ItemSystem::ItemSystem(LayerManager* layerManager)
	: m_layerManager(layerManager)
{
}

ItemSystem::~ItemSystem()
{
	GameGlobals::events->unsubscribe<entityx::EntityDestroyedEvent>(*this);
	GameGlobals::events->unsubscribe<ItemPickedUpEvent>(*this);
}

void ItemSystem::addedToEngine(Engine *engine)
{
	events.subscribe<entityx::EntityDestroyedEvent>(*this);
	events.subscribe<ItemPickedUpEvent>(*this);
}

void ItemSystem::update(float dt)
{
	for (auto item : entityManager.entities_with_components<ItemComponent, CellComponent, LayerComponent>())
	{
		if (!item->isValid() || item->has<DestructionComponent>())
			continue;

		auto cellComponent = item->get<CellComponent>();
		auto layerComponent = item->get<LayerComponent>();
		auto itemComponent = item->get<ItemComponent>();

		auto entityWithInventory = m_layerManager->getEntityWithComponent<InventoryComponent>(layerComponent->layer, cellComponent->x, cellComponent->y);
		if (entityWithInventory->isValid())
		{
			auto inventory = entityWithInventory->get<InventoryComponent>();

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
			default: break;
			}

			eventManager.emit<ItemPickedUpEvent>(item, entityWithInventory);
		}
	}
}

void ItemSystem::onEntityDestroyed(Entity *entity)
{
	if (!entity->isValid())
		return;

	if (entity->has<BlockComponent>())
	{
		auto cell = entity->get<CellComponent>();
		assert(cell);

		if (Random::getInt(1, 100) <= 33) // 33% Chance to spawn an item
		{
			// TODO: Create random items taking rarity and minimum spawn number into consideration, 
			// assigning ItemComponents with blocks during Level creation might be a good idea
			GameGlobals::entityFactory->createItem(cell->x, cell->y, ItemType::BOMB_CAP_BOOST);
		} 
		else if (Random::getInt(1, 100) <= 33)
		{
			GameGlobals::entityFactory->createItem(cell->x, cell->y, ItemType::BOMB_KICK_SKILL);
		}
		else if (Random::getInt(1, 100) <= 33)
		{
			GameGlobals::entityFactory->createItem(cell->x, cell->y, ItemType::SPEED_MULTIPLICATOR);
		}
	}
}

void ItemSystem::onItemPickedUp(Entity *item, Entity *itemReceiver)
{
	if (!item->isValid())
		return;

	item.assign<DestructionComponent>(0.f);
}