#pragma once
#include <entityx/System.h>
#include "../EntityFactory.h"

class InputHandleSystem : public entityx::System<InputHandleSystem>
{
public:
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
};

