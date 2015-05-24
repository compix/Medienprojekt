#pragma once
#include <entityx/entityx.h>
#include "../EntityLayer.h"

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class InputManager;

class InputSystem : public System<InputSystem>
{
public:
	InputSystem(InputManager &inputManager);

	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;

private:
	InputManager &inputManager;
};

