#pragma once
#include <entityx/entityx.h>

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class BodySystem : public System<BodySystem>
{
public:
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
};