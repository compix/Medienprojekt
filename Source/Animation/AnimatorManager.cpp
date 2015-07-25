#include "AnimatorManager.h"
#include "WalkingState.h"
#include "IdleState.h"

std::unordered_map<AnimatorType, std::unique_ptr<Animator>, EnumClassHash> AnimatorManager::m_animators;

void AnimatorManager::init()
{
	initCharacterAnimator(AnimatorType::PLAYER0, "player0");
	initCharacterAnimator(AnimatorType::PLAYER1, "player1");
	initCharacterAnimator(AnimatorType::PLAYER2, "player2");
	initCharacterAnimator(AnimatorType::PLAYER3, "player3");
}

void AnimatorManager::assignCharacterAnimator(entityx::Entity& entity, int playerIndex)
{
	auto animationComponent = entity.component<AnimationComponent>();
	assert(animationComponent);

	auto animator = m_animators[(AnimatorType)((int)AnimatorType::PLAYER0 + playerIndex)].get();
	animator->changeTo<IdleState>(entity);
	animationComponent->animator = animator;
}

void AnimatorManager::initCharacterAnimator(AnimatorType type, const std::string& textureName)
{
	auto animator = std::make_unique<Animator>();

	animator->add(AnimationType::WALK_DOWN, "char_walk_down", textureName);
	animator->add(AnimationType::WALK_UP, "char_walk_up", textureName);
	animator->add(AnimationType::WALK_LEFT, "char_walk_left", textureName);
	animator->add(AnimationType::WALK_RIGHT, "char_walk_right", textureName);

	animator->add(AnimationType::IDLE_DOWN, "char_idle_down", textureName);
	animator->add(AnimationType::IDLE_UP, "char_idle_up", textureName);
	animator->add(AnimationType::IDLE_LEFT, "char_idle_left", textureName);
	animator->add(AnimationType::IDLE_RIGHT, "char_idle_right", textureName);

	animator->add<IdleState>();
	animator->add<WalkingState>();

	m_animators[type] = std::move(animator);
}