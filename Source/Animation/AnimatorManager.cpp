#include "AnimatorManager.h"
#include "WalkingState.h"
#include "IdleState.h"

std::unordered_map<AnimatorType, std::unique_ptr<Animator>, EnumClassHash> AnimatorManager::m_animators;

void AnimatorManager::init()
{
	initCharacterAnimator();
}

void AnimatorManager::assignCharacterAnimator(entityx::Entity& entity)
{
	auto animationComponent = entity.component<AnimationComponent>();
	assert(animationComponent);

	auto animator = m_animators[AnimatorType::CHARACTER].get();
	animator->changeTo<IdleState>(entity);
	animationComponent->animator = animator;
}

void AnimatorManager::initCharacterAnimator()
{
	auto animator = std::make_unique<Animator>();

	animator->add(AnimationType::WALK_DOWN, "char_walk_down", "goku");
	animator->add(AnimationType::WALK_UP, "char_walk_up", "goku");
	animator->add(AnimationType::WALK_LEFT, "char_walk_left", "goku");
	animator->add(AnimationType::WALK_RIGHT, "char_walk_right", "goku");

	animator->add(AnimationType::IDLE_DOWN, "char_idle_down", "goku");
	animator->add(AnimationType::IDLE_UP, "char_idle_up", "goku");
	animator->add(AnimationType::IDLE_LEFT, "char_idle_left", "goku");
	animator->add(AnimationType::IDLE_RIGHT, "char_idle_right", "goku");

	animator->add<IdleState>();
	animator->add<WalkingState>();

	m_animators[AnimatorType::CHARACTER] = std::move(animator);
}