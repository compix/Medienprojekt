#pragma once
#include "AnimationState.h"

class IdleState : public AnimationState
{
public:
	void update(Animator* animator, Entity *entity, float deltaTime) override;
};