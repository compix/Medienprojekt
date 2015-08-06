#include "InputHandleSystem.h"
#include "../Components/TimerComponent.h"
#include "../Components/InputComponent.h"
#include "../GameConstants.h"
#include <Box2D/Common/b2Math.h>
#include "../Components/BodyComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../GameGlobals.h"
#include "../Components/DirectionComponent.h"

InputHandleSystem::InputHandleSystem()
: IteratingSystem(Family::all<InputComponent, CellComponent, InventoryComponent>().get()) {}

void InputHandleSystem::processEntity(Entity *entity, float deltaTime)
{
	auto input = entity->get<InputComponent>();
	auto cell = entity->get<CellComponent>();
	auto inventory = entity->get<InventoryComponent>();

	if (input->bombButtonPressed)
	{
		if (inventory->bombCount > 0)
		{
			GameGlobals::entityFactory->createBomb(cell->x, cell->y, entity);
			inventory->bombCount--;
		}

		input->bombButtonPressed = false;
	}

	if (input->skillButtonPressed)
	{
		GameGlobals::events->createPortal.emit(entity);
		input->skillButtonPressed = false;
	}

	auto body = entity->get<BodyComponent>();
	if (body)
		body->body->SetLinearVelocity(b2Vec2(input->moveX * (GameConstants::PLAYER_SPEED*inventory->speedMultiplicator), input->moveY * (GameConstants::PLAYER_SPEED*inventory->speedMultiplicator)));
}