#pragma once
#include <entityx/entityx.h>
#include "EntityLayer.h"

using namespace entityx;

class InputManager;

class InputSystem : public System<InputSystem>
{
public:
	InputSystem(InputManager &inputManager);

	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;

private:
	InputManager &inputManager;
};

