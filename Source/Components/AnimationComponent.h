#pragma once
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

struct AnimationComponent
{
	AnimationComponent() : timeTillNextFrame(0.f), playMode(PlayMode::NORMAL), currentFrame(0), startFrame(0), currentAnimation(AnimationType::NONE) {}

	sf::IntRect baseRect;
	uint8_t colCount;
	uint8_t rowCount;
	uint8_t frameCount;
	float frameDuration;
	float timeTillNextFrame;
    PlayMode playMode;
    int8_t currentFrame;
	uint8_t startFrame;
    AnimationType currentAnimation;

	Animator* animator;
	AnimationState* state;
};
