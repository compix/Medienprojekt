#pragma once
#include <ecstasy/core/EntitySystem.h>

using namespace ECS;

class AnimationSystem : public EntitySystem<AnimationSystem>
{
public:
	void update(float dt) override;
};

