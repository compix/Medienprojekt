#include "InputSystem.h"
#include "../Utils/InputManager.h"
#include "../Components/InputComponent.h"
#include "../Components/AIComponent.h"
#include "../GameGlobals.h"

void InputSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	auto entities = entityManager.entities_with_components<InputComponent>();
	for (auto entity : entities)
	{
		auto input = entity.component<InputComponent>();
		if (input->playerIndex < 0 || entity.has_component<AIComponent>())
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
		
		// Normalize (in case someone is cheating)
		if (input->moveX || input->moveY)
		{
			float len = sqrtf(input->moveX * input->moveX + input->moveY * input->moveY);
			if(len > 1) {
				input->moveX /= len;
				input->moveY /= len;
			}
		}
	}
}
