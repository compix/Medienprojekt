#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/DirectionComponent.h"
#include <math.h>
#include <algorithm>
#include "../Animation/Animator.h"

void AnimationSystem::addedToEngine(Engine *engine) {
	m_inputEntities = engine->getEntitiesFor(Family::all<InputComponent, DirectionComponent>().get());
	m_animationEntities = engine->getEntitiesFor(Family::all<AnimationComponent, SpriteComponent>().get());
}

void AnimationSystem::update(float dt)
{
	for (Entity *e : *m_inputEntities)
	{
		auto input = e->get<InputComponent>();
		auto directionComponent = e->get<DirectionComponent>();
		if (directionComponent)
		{
			if (abs(input->moveX) >= abs(input->moveY))
			{
				if (input->moveX > 0)
					directionComponent->direction = Direction::RIGHT;
				else if (input->moveX < 0)
					directionComponent->direction = Direction::LEFT;
			}
			else
			{
				if (input->moveY > 0)
					directionComponent->direction = Direction::DOWN;
				else if (input->moveY < 0)
					directionComponent->direction = Direction::UP;
			}
		}
	}
	
	for (Entity *entity : *m_animationEntities)
	{
		auto animation = entity->get<AnimationComponent>();
		auto sprite = entity->get<SpriteComponent>();
		animation->animator->update(entity, (float)dt);

		int frameWidth = animation->baseRect.width / animation->colCount;
		int frameHeight = animation->baseRect.height / animation->rowCount;

		bool newFrame = false;
		animation->timeTillNextFrame -= (float) dt;
		int frameSkip = 0;

		assert(animation->frameDuration > 0.f);

		while (animation->timeTillNextFrame <= 0.f)
		{
			animation->timeTillNextFrame += animation->frameDuration;
			newFrame = true;
			++frameSkip;
		}

		switch (animation->playMode)
		{
		case NORMAL:
			if (newFrame)
			{
				animation->currentFrame += frameSkip;
				if (animation->currentFrame > animation->startFrame + animation->frameCount - 1)
					animation->currentFrame = animation->startFrame + animation->frameCount - 1;
			}
			break;
		case LOOP:
			if (newFrame)
			{
				animation->currentFrame += frameSkip;
				if (animation->currentFrame > animation->startFrame + animation->frameCount - 1)
					animation->currentFrame = animation->startFrame;
			}
			break;
		case REVERSED:
			if (newFrame)
			{
				animation->currentFrame -= frameSkip;
				if (animation->currentFrame < animation->startFrame)
					animation->currentFrame = animation->startFrame;
			}
			break;
		case LOOP_REVERSED:
			if (newFrame)
			{
				animation->currentFrame -= frameSkip;
				if (animation->currentFrame < animation->startFrame)
					animation->currentFrame = animation->startFrame + animation->frameCount - 1;
			}
			break;
		}

		int row = animation->currentFrame / animation->colCount;
		int col = animation->currentFrame % animation->colCount;

		int rectStartX = col*frameWidth;
		int rectStartY = row*frameHeight;

		auto& baseRect = animation->baseRect;
		sprite->sprite.setTextureRect(sf::IntRect(baseRect.left+rectStartX, baseRect.top+rectStartY, frameWidth, frameHeight));
		sprite->sprite.setOrigin(frameWidth * 0.5f, frameHeight * 0.5f);
	}
}
