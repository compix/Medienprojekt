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
	AnimationComponent() : timeTillNextFrame(0.f), playMode(PlayMode::NORMAL), currentFrame(0), startFrame(0), currentAnimation(AnimationType::NONE) {}

	sf::IntRect baseRect;
	uint8_t colCount = 0;
	uint8_t rowCount = 0;
	uint8_t frameCount = 0;
	float frameDuration = 0;
	float timeTillNextFrame;
	uint8_t startFrame;
	int8_t currentFrame;
	PlayMode playMode;

	Animator* animator = nullptr;
	AnimationState* state = nullptr;
	AnimationType currentAnimation;
};