#include "WalkingState.h"
#include "../Components/DirectionComponent.h"
#include "../Utils/Math.h"
#include "../Components/InputComponent.h"
#include "IdleState.h"

void WalkingState::update(Animator* animator, entityx::Entity& entity, float deltaTime)
{
	auto directionComponent = entity.component<DirectionComponent>();
	auto inputComponent = entity.component<InputComponent>();

	if (inputComponent)
	{
		if (Math::nearEq(inputComponent->moveX, 0.f) && Math::nearEq(inputComponent->moveY, 0.f))
		{
			animator->changeTo<IdleState>(entity);
			return;
		}
	}

	if (!directionComponent)
		return;

	switch (directionComponent->direction)
	{
	case Direction::UP:
		animator->updateAnimation(AnimationType::WALK_UP, entity);
		break;
	case Direction::DOWN:
		animator->updateAnimation(AnimationType::WALK_DOWN, entity);
		break;
	case Direction::LEFT:
		animator->updateAnimation(AnimationType::WALK_LEFT, entity);
		break;
	case Direction::RIGHT:
		animator->updateAnimation(AnimationType::WALK_RIGHT, entity);
		break;
	}
}