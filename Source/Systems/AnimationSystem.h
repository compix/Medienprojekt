#pragma once
#include <entityx/entityx.h>

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class AnimationSystem : public System<AnimationSystem>
{
public:
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
};

