#pragma once
#include <ecstasy/core/Component.h>
#include <stdint.h>
#include <SFML/Graphics.hpp>
#include <memory>
#include "../Animation/AnimationType.h"

class Animator;
class AnimationState;

enum PlayMode
{
	NORMAL,
	REVERSED,
	LOOP,
	LOOP_REVERSED
};

struct AnimationComponent: public Component<AnimationComponent>
{
	sf::IntRect baseRect;
	uint8_t colCount = 0;
	uint8_t rowCount = 0;
	uint8_t frameCount = 0;
	float frameDuration = 0;
	float timeTillNextFrame = 0;
	uint8_t startFrame = 0;
	int8_t currentFrame = 0;
	PlayMode playMode = PlayMode::NORMAL;

	Animator* animator = nullptr;
	AnimationState* state = nullptr;
	AnimationType currentAnimation = AnimationType::NONE;
	
	void reset() override {
		baseRect.left = 0;
		baseRect.top = 0;
		baseRect.width = 0;
		baseRect.height = 0;
		colCount = 0;
		rowCount = 0;
		frameCount = 0;
		frameDuration = 0;
		timeTillNextFrame = 0;
		startFrame = 0;
		currentFrame = 0;
		playMode = PlayMode::NORMAL;

		animator = nullptr;
		state = nullptr;
		currentAnimation = AnimationType::NONE;
	}
};