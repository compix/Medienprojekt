#pragma once
#include "AnimationState.h"

class IdleState : public AnimationState
{
public:
	void update(Animator* animator, entityx::Entity& entity, float deltaTime) override;
};