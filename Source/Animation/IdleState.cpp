#include "IdleState.h"
#include "../Components/DirectionComponent.h"
#include "../Components/InputComponent.h"
#include "../Utils/Math.h"
#include "WalkingState.h"

void IdleState::update(Animator* animator, entityx::Entity& entity, float deltaTime)
{
	auto directionComponent = entity.component<DirectionComponent>();
	auto inputComponent = entity.component<InputComponent>();

	if (inputComponent)
	{
		if (!Math::nearEq(inputComponent->moveX, 0.f) || !Math::nearEq(inputComponent->moveY, 0.f))
		{
			animator->changeTo<WalkingState>(entity);
			animator->update(entity, deltaTime);
			return;
		}		
	}

	if (!directionComponent)
		return;

	switch (directionComponent->direction)
	{
	case Direction::UP:
		animator->updateAnimation(AnimationType::IDLE_UP, entity);
		break;
	case Direction::DOWN:
		animator->updateAnimation(AnimationType::IDLE_DOWN, entity);
		break;
	case Direction::LEFT:
		animator->updateAnimation(AnimationType::IDLE_LEFT, entity);
		break;
	case Direction::RIGHT:
		animator->updateAnimation(AnimationType::IDLE_RIGHT, entity);
		break;
	default: break;
	}
}