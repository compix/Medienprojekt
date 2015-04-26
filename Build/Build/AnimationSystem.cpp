#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "SpriteComponent.h"
#include <math.h>
#include <algorithm>

void AnimationSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<AnimationComponent> animation;
	ComponentHandle<SpriteComponent> sprite;

	for (Entity entity : entityManager.entities_with_components(animation, sprite))
	{
		int frameWidth = sprite->sprite.getTexture()->getSize().x / animation->colCount;
		int frameHeight = sprite->sprite.getTexture()->getSize().y / animation->rowCount;

		float animationDuration = animation->frameCount * animation->frameDuration;

		int frame = animation->stateTime / animationDuration * animation->frameCount;

		animation->stateTime += dt;

		switch (animation->playMode)
		{
		case NORMAL:
			frame = std::min(animation->frameCount - 1, frame);
			break;
		case LOOP:
			if (animation->stateTime >= animationDuration)
				animation->stateTime = animation->stateTime - animationDuration;
			break;
		case REVERSED:
			frame = (animationDuration - animation->stateTime) / animationDuration * animation->frameCount;
			frame = std::max(frame, 0);
			break;
		case LOOP_REVERSED:
			if (animation->stateTime >= animationDuration)
				animation->stateTime = animation->stateTime - animationDuration;

			frame = (animationDuration - animation->stateTime) / animationDuration * animation->frameCount;
			frame = std::max(frame, 0);
			break;
		case PING_PONG:
			if (animation->stateTime >= animationDuration)
			{
				float stateTime = animation->stateTime - animationDuration;
				frame = (animationDuration - stateTime) / animationDuration * animation->frameCount;
				frame = std::max(frame, 0);
			}
			break;
		case LOOP_PING_PONG:
			if (animation->stateTime >= animationDuration)
			{
				float stateTime = animation->stateTime - animationDuration;
				frame = (animationDuration - stateTime) / animationDuration * animation->frameCount;
				if (animation->stateTime >= animationDuration*2.f)
					animation->stateTime = animation->stateTime - animationDuration*2.f;
			}
			break;
		}

		int row = frame / animation->colCount;
		int col = frame % animation->colCount;

		int rectStartX = (int)col*frameWidth;
		int rectStartY = (int)row*frameHeight;

		sprite->sprite.setTextureRect(sf::IntRect(rectStartX, rectStartY, frameWidth, frameHeight));
	}
}
