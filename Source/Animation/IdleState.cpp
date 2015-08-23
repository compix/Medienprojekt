#include "IdleState.h"
#include "../Components/DirectionComponent.h"
#include "../Components/InputComponent.h"
#include "../Utils/Math.h"
#include "WalkingState.h"
#include "../Components/InventoryComponent.h"

void IdleState::update(Animator* animator, entityx::Entity& entity, float deltaTime)
{
	auto directionComponent = entity.component<DirectionComponent>();
	auto inputComponent = entity.component<InputComponent>();
	auto inventoryComponent = entity.component<InventoryComponent>();

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
		if (!inventoryComponent->isHoldingBomb){
			animator->updateAnimation(AnimationType::IDLE_UP, entity);
		} else
		{
			animator->updateAnimation(AnimationType::IDLE_UP_BOMB, entity);
		}
		break;
	case Direction::DOWN:
		if (!inventoryComponent->isHoldingBomb){
			animator->updateAnimation(AnimationType::IDLE_DOWN, entity);
		}
		else{
			animator->updateAnimation(AnimationType::IDLE_DOWN_BOMB, entity);
		}
		break;
	case Direction::LEFT:
		if (!inventoryComponent->isHoldingBomb){
			animator->updateAnimation(AnimationType::IDLE_LEFT, entity);
		}
		else{
			animator->updateAnimation(AnimationType::IDLE_LEFT_BOMB, entity);
		}
		break;
	case Direction::RIGHT:
		if (!inventoryComponent->isHoldingBomb){
			animator->updateAnimation(AnimationType::IDLE_RIGHT, entity);
		}
		else{
			animator->updateAnimation(AnimationType::IDLE_RIGHT_BOMB, entity);
		}
		break;
	default: break;
	}
}