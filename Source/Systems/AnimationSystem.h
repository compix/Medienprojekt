#pragma once
#include <entityx/entityx.h>

using namespace entityx;

class AnimationSystem : public System<AnimationSystem>
{
public:
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
};

