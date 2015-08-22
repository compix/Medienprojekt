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
			if (inventory->bombCount > 0 && !m_layerManager->hasEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
			{
				GameGlobals::entityFactory->createBomb(cell->x, cell->y, entity);
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
			input->skillButtonPressed = false;
		}

		auto body = entity.component<BodyComponent>();
		if (body)
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