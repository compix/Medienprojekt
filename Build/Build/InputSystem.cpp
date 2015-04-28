#include "InputSystem.h"
#include "InputManager.h"
#include "InputComponent.h"
#include <iostream>

using namespace std;

InputSystem::InputSystem(InputManager &inputManager) : inputManager(inputManager)
{
}

void InputSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	auto entities = entityManager.entities_with_components<InputComponent>();
	for (auto entity : entities)
	{
		auto input = entity.component<InputComponent>();

		auto &playerInput = inputManager.getPlayerInput(input->playerIndex);
		if (playerInput.buttonPressed[PlayerButton::BOMB])
		{
			input->bombButtonPressed = true;
			playerInput.buttonPressed[PlayerButton::BOMB] = false;
			cout << "Bomb triggered" << endl;
		}
		if (playerInput.buttonPressed[PlayerButton::SKILL])
		{
			input->skillButtonPressed = true;
			playerInput.buttonPressed[PlayerButton::SKILL] = false;
			cout << "Skill triggered" << endl;
		}
		input->moveX = playerInput.moveX;
		input->moveX = playerInput.moveY;
	}
}
