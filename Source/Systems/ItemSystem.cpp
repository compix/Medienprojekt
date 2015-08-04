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
	: IteratingSystem(Family::all<ItemComponent, CellComponent, LayerComponent>().get()), m_layerManager(layerManager)
{
}

ItemSystem::~ItemSystem()
{
	m_connections.removeAll();
}

void ItemSystem::addedToEngine(Engine *engine)
{
	IteratingSystem::addedToEngine(engine);
	m_connections += engine->entityRemoved.connect(this, &ItemSystem::onEntityDestroyed);
	m_connections += GameGlobals::events->itemPickedUp.connect(this, &ItemSystem::onItemPickedUp);
}

void ItemSystem::processEntity(Entity *item, float deltaTime)
{
	if (!item->isValid() || item->has<DestructionComponent>())
		return;

	auto cellComponent = item->get<CellComponent>();
	auto layerComponent = item->get<LayerComponent>();
	auto itemComponent = item->get<ItemComponent>();

	auto entityWithInventory = m_layerManager->getEntityWithComponent<InventoryComponent>(layerComponent->layer, cellComponent->x, cellComponent->y);
	if (entityWithInventory && entityWithInventory->isValid())
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

		GameGlobals::events->itemPickedUp.emit(item, entityWithInventory);
	}
}

void ItemSystem::onEntityDestroyed(Entity *entity)
{
	if (!entity->isValid() || !GameGlobals::entityFactory)
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

	item->add(getEngine()->createComponent<DestructionComponent>());
}