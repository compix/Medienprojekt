#include "InputSystem.h"
#include "../Utils/InputManager.h"
#include "../Components/InputComponent.h"
#include <iostream>
#include "../Components/BodyComponent.h"
#include "../GameConstants.h"
#include "../GameGlobals.h"

using namespace std;

void InputSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	auto entities = entityManager.entities_with_components<InputComponent, BodyComponent>();
	for (auto entity : entities)
	{
		auto input = entity.component<InputComponent>();

		auto &playerInput = GameGlobals::input->getPlayerInput(input->playerIndex);
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
		input->moveY = playerInput.moveY;
	}
}
