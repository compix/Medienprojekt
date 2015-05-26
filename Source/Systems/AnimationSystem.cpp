#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"
#include <math.h>
#include <algorithm>

void AnimationSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<AnimationComponent> animation;
	ComponentHandle<SpriteComponent> sprite;

	for (Entity entity : entityManager.entities_with_components(animation, sprite))
	{
		int frameWidth = animation->baseRect.width / animation->colCount;
		int frameHeight = animation->baseRect.height / animation->rowCount;

		float animationDuration = animation->frameCount * animation->frameDuration;

		int frame = (int) (animation->stateTime / animationDuration * animation->frameCount);

		animation->stateTime += (float) dt;

		switch (animation->playMode)
		{
		case NORMAL:
			frame = std::min(animation->frameCount - 1, frame);
			break;
		case LOOP:
			if (animation->stateTime >= animationDuration)
				animation->stateTime -= animationDuration;
			break;
		case REVERSED:
			frame = (int) ((animationDuration - animation->stateTime) / animationDuration * animation->frameCount);
			frame = std::max(frame, 0);
			break;
		case LOOP_REVERSED:
			if (animation->stateTime >= animationDuration)
				animation->stateTime -= animationDuration;

			frame = (int) ((animationDuration - animation->stateTime) / animationDuration * animation->frameCount);
			frame = std::max(frame, 0);
			break;
		case PING_PONG:
			if (animation->stateTime >= animationDuration)
			{
				float stateTime = animation->stateTime - animationDuration;
				frame = (int) ((animationDuration - stateTime) / animationDuration * animation->frameCount);
				frame = std::max(frame, 0);
			}
			break;
		case LOOP_PING_PONG:
			if (animation->stateTime >= animationDuration)
			{
				float stateTime = animation->stateTime - animationDuration;
				frame = (int) ((animationDuration - stateTime) / animationDuration * animation->frameCount);
				if (animation->stateTime >= animationDuration * 2.f)
					animation->stateTime -= animationDuration * 2.f;
			}
			break;
		}

		int row = frame / animation->colCount;
		int col = frame % animation->colCount;

		int rectStartX = col*frameWidth;
		int rectStartY = row*frameHeight;

		auto& baseRect = animation->baseRect;
		sprite->sprite.setTextureRect(sf::IntRect(baseRect.left+rectStartX, baseRect.top+rectStartY, frameWidth, frameHeight));
		sprite->sprite.setOrigin(frameWidth * 0.5f, frameHeight * 0.5f);
	}
}
