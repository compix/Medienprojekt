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
		auto body = entity.component<BodyComponent>();

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

		if (playerInput.buttonPressed[PlayerButton::RIGHT]){
			input->moveX = true;
			playerInput.buttonPressed[PlayerButton::RIGHT] = false;
			body->body->SetLinearVelocity(b2Vec2(GameConstants::PLAYER_SPEED, 0));
		}

		if (playerInput.buttonPressed[PlayerButton::DOWN]){
			input->moveX = true;
			playerInput.buttonPressed[PlayerButton::DOWN] = false;
			body->body->SetLinearVelocity(b2Vec2(0, GameConstants::PLAYER_SPEED));
		}

		if (playerInput.buttonPressed[PlayerButton::LEFT]){
			input->moveX = true;
			playerInput.buttonPressed[PlayerButton::LEFT] = false;
			body->body->SetLinearVelocity(b2Vec2(-GameConstants::PLAYER_SPEED, 0));
		}

		if (playerInput.buttonPressed[PlayerButton::UP]){
			input->moveX = true;
			playerInput.buttonPressed[PlayerButton::UP] = false;
			body->body->SetLinearVelocity(b2Vec2(0, -GameConstants::PLAYER_SPEED));
		}
		input->moveX = playerInput.moveX;
		input->moveX = playerInput.moveY;
	}
}
