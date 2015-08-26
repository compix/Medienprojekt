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
#include "../Components/PortalComponent.h"
#include "../Components/BombComponent.h"
#include "../Events/HoldingEvent.h"
#include "../Events/ThrowBombEvent.h"
#include "../Components/BlinkComponent.h"
#include "../Events/SkillEvent.h"

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
			activateSkill(entity);
			input->skillButtonPressed = false;
		}

		auto body = entity.component<BodyComponent>();
		if (body && !entity.has_component<BlinkComponent>())
			body->body->SetLinearVelocity(b2Vec2(input->moveX * (GameConstants::PLAYER_SPEED*inventory->speedMultiplicator), input->moveY * (GameConstants::PLAYER_SPEED*inventory->speedMultiplicator)));
	}
}

void InputHandleSystem::activateSkill(entityx::Entity& entity)
{
	auto inventory = entity.component<InventoryComponent>();
	auto cell = entity.component<CellComponent>();
	assert(inventory && cell);
	SkillType type = inventory->activeSkill();
	switch (type)
	{
	case SkillType::PLACE_PORTAL:
		// If both portals have been placed already or there is a portal on the current cell, do nothing
		if (inventory->placedPortals.second.valid() ||
			m_layerManager->hasEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
			return;
		break;
	case SkillType::PUNCH:
		break;
	case SkillType::BLINK:
		if (entity.has_component<BlinkComponent>())
			return;
		break;
	default:
		return;
	}
	GameGlobals::events->emit<SkillEvent>(entity, type, true);
}
