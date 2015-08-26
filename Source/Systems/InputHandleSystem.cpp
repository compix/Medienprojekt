#include "InputHandleSystem.h"
#include "../Components/TimerComponent.h"
#include "../Events/TimeoutEvent.h"
#include "../Components/InputComponent.h"
#include "../GameConstants.h"
#include <Box2D/Common/b2Math.h>
#include "../Components/BodyComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../GameGlobals.h"
#include "../Components/DirectionComponent.h"
#include "../Events/CreatePortalEvent.h"
#include "../Components/PortalComponent.h"
#include "../Components/BombComponent.h"
#include "../Events/PunchEvent.h"
#include "../Events/HoldingEvent.h"
#include "../Events/ThrowBombEvent.h"
#include "../Components/BlinkComponent.h"

InputHandleSystem::InputHandleSystem(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
}


void InputHandleSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<InputComponent, CellComponent, InventoryComponent>())
	{
		auto input = entity.component<InputComponent>();
		auto cell = entity.component<CellComponent>();
		auto inventory = entity.component<InventoryComponent>();

		if (input->bombButtonPressed)
		{
			Entity belowBomb;
			if (inventory->canHold && (belowBomb = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y)) && !inventory->isHoldingBomb)
			{
				GameGlobals::events->emit<HoldingEvent>(entity, belowBomb);
			}
			else if (inventory->isHoldingBomb)
			{
				GameGlobals::events->emit<ThrowBombEvent>(entity);
			}
			if (inventory->bombCount > 0 && !m_layerManager->hasEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y) && !inventory->isHoldingBomb)
			{
				GameGlobals::entityFactory->createBomb(cell->x, cell->y, entity, inventory->activeBomb());
				inventory->bombCount--;
			}
			
			input->bombButtonPressed = false;
		}

		if (input->skillButtonPressed)
		{
			if (canPlacePortal(entity))
				GameGlobals::events->emit<CreatePortalEvent>(entity);
			if (canPunch(entity))
				GameGlobals::events->emit<PunchEvent>(entity, GameConstants::PUNCH_DISTANCE);
			if (canBlink(entity))
				if (!entity.has_component<BlinkComponent>()){
					entity.assign<BlinkComponent>();
				}
			input->skillButtonPressed = false;
		}

		auto body = entity.component<BodyComponent>();
		if (body && !entity.has_component<BlinkComponent>())
			body->body->SetLinearVelocity(b2Vec2(input->moveX * (GameConstants::PLAYER_SPEED*inventory->speedMultiplicator), input->moveY * (GameConstants::PLAYER_SPEED*inventory->speedMultiplicator)));
	}
}

bool InputHandleSystem::canPlacePortal(entityx::Entity& entity)
{
	auto inventory = entity.component<InventoryComponent>();
	auto cell = entity.component<CellComponent>();
	assert(inventory && cell);
	return inventory->isActive(SkillType::PLACE_PORTAL) && // Portal skill is active
		   !inventory->placedPortals.second.valid()     && // Didn't place both portals yet
		   !m_layerManager->hasEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y); // No portal on the current cell
}

bool InputHandleSystem::canPunch(entityx::Entity& entity)
{
	auto inventory = entity.component<InventoryComponent>();
	auto cell = entity.component<CellComponent>();
	assert(inventory && cell);
	return inventory->isActive(SkillType::PUNCH);
}

bool InputHandleSystem::canBlink(entityx::Entity& entity)
{
	auto inventory = entity.component<InventoryComponent>();
	auto cell = entity.component<CellComponent>();
	assert(inventory && cell);
	return inventory->isActive(SkillType::BLINK);
}