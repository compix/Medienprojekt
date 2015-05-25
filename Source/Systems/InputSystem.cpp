#include "InputSystem.h"
#include "../Utils/InputManager.h"
#include "../Components/InputComponent.h"
#include "../GameGlobals.h"

void InputSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	auto entities = entityManager.entities_with_components<InputComponent>();
	for (auto entity : entities)
	{
		auto input = entity.component<InputComponent>();
		if (input->playerIndex < 0)
			continue;

		auto &playerInput = GameGlobals::input->getPlayerInput(input->playerIndex);
		if (playerInput.buttonPressed[PlayerButton::BOMB])
		{
			input->bombButtonPressed = true;
			playerInput.buttonPressed[PlayerButton::BOMB] = false;
		}
		if (playerInput.buttonPressed[PlayerButton::SKILL])
		{
			input->skillButtonPressed = true;
			playerInput.buttonPressed[PlayerButton::SKILL] = false;
		}

		input->moveX = playerInput.moveX;
		input->moveY = playerInput.moveY;
	}
}
