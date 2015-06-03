#pragma once
#include "entityx/entityx.h"
#include "Animator.h"

class AnimationState
{
public:
	virtual ~AnimationState(){}

	virtual void onEnter() {};
	virtual void onExit() {};
	virtual void update(Animator* animator, entityx::Entity& entity, float deltaTime) = 0;
};