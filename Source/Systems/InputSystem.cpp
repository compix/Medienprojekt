#include "InputSystem.h"
#include "../Utils/InputManager.h"
#include "../Components/InputComponent.h"
#include "../Components/AIComponent.h"
#include "../GameGlobals.h"
#include "../Components/LocalInputComponent.h"

InputSystem::InputSystem()
: IteratingSystem(Family::all<InputComponent, LocalInputComponent>().get()) {}

void InputSystem::processEntity(Entity *entity, float deltaTime)
{
	auto localInput = entity->get<LocalInputComponent>();
	auto input = entity->get<InputComponent>();
	auto &playerInput = GameGlobals::input->getPlayerInput(localInput->inputIndex);
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
