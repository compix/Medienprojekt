#include "Animator.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"
#include "AnimationState.h"
#include "../Components/SpriteComponent.h"
#include "../Utils/AssetManagement/AnimationLoader.h"

void Animator::add(AnimationType animationType, const std::string& animationName, const std::string& textureName)
{
	auto animation = GameGlobals::assetManager->getAnimation(animationName);
	auto texture = GameGlobals::assetManager->getTexture(textureName);

	m_animations[animationType] = AnimationInfo(animation, texture);
}

const AnimationInfo& Animator::get(AnimationType animationType)
{
	assert(m_animations.count(animationType) > 0);
	return m_animations[animationType];
}

void Animator::updateAnimation(AnimationType animationType, Entity *entity)
{
	auto animationComponent = entity->get<AnimationComponent>();
	auto spriteComponent = entity->get<SpriteComponent>();

	assert(animationComponent && spriteComponent);

	if (animationComponent->currentAnimation == animationType)
		return;

	const AnimationInfo& animInfo = get(animationType);
	auto tex = animInfo.m_texture;
	auto anim = animInfo.m_animation;

	spriteComponent->sprite.setTexture(*tex->get());
	spriteComponent->sprite.setOrigin(tex->getRect().width*0.5f, tex->getRect().height*0.5f);
	spriteComponent->sprite.setTextureRect(tex->getRect());

	animationComponent->playMode = anim->playMode;
	animationComponent->colCount = anim->colCount;
	animationComponent->rowCount = anim->rowCount;
	animationComponent->frameCount = anim->frameCount;
	animationComponent->frameDuration = anim->frameDuration;
	animationComponent->startFrame = anim->startFrame;
	animationComponent->baseRect = spriteComponent->sprite.getTextureRect();
	animationComponent->timeTillNextFrame = anim->frameDuration;
	animationComponent->currentAnimation = animationType;

	switch (anim->playMode)
	{
	case REVERSED:
		animationComponent->currentFrame = anim->startFrame + anim->frameCount - 1;
		break;
	case LOOP_REVERSED:
		animationComponent->currentFrame = anim->startFrame + anim->frameCount - 1;
		break;
	default:
		animationComponent->currentFrame = anim->startFrame;
		break;
	}
}

void Animator::update(Entity *entity, float deltaTime)
{
	auto animationComponent = entity->get<AnimationComponent>();

	if (animationComponent->state)
		animationComponent->state->update(this, entity, deltaTime);
}