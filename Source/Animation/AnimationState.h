#pragma once
#include "ecstasy/core/Entity.h"
#include "Animator.h"

using namespace ECS;

class AnimationState
{
public:
	virtual ~AnimationState(){}

	virtual void onEnter() {};
	virtual void onExit() {};
	virtual void update(Animator* animator, Entity *entity, float deltaTime) = 0;
};