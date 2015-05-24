#pragma once
#include <entityx/entityx.h>
#include "../EntityLayer.h"

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class InputSystem : public System<InputSystem>
{
public:
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
};

