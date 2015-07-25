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
#include "../Components/BombComponent.h"

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

		auto body = entity.component<BodyComponent>();
		if (body)
			body->body->SetLinearVelocity(b2Vec2(input->moveX * GameConstants::PLAYER_SPEED, input->moveY * GameConstants::PLAYER_SPEED));
	}
}