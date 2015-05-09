#include "InputSystem.h"
#include "../Utils/InputManager.h"
#include "../Components/InputComponent.h"
#include <iostream>
#include "../Components/BodyComponent.h"
#include <GameConstants.h>

using namespace std;

InputSystem::InputSystem(InputManager &inputManager) : inputManager(inputManager) { }

void InputSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	auto entities = entityManager.entities_with_components<InputComponent, BodyComponent>();
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
		input->moveY = playerInput.moveY;

		//fixme: this should be in a separate system
		auto body = entity.component<BodyComponent>();
		body->body->SetLinearVelocity(b2Vec2(playerInput.moveX * GameConstants::PLAYER_SPEED, playerInput.moveY * GameConstants::PLAYER_SPEED));
	}
}
