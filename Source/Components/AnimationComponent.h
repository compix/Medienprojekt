#pragma once

enum PlayMode
{
	NORMAL,
	REVERSED,
	LOOP,
	LOOP_REVERSED,
	PING_PONG,
	LOOP_PING_PONG
};

struct AnimationComponent
{
	AnimationComponent() : stateTime(0.f), playMode(PlayMode::NORMAL) {}

	sf::IntRect baseRect;
	int colCount;
	int rowCount;
	int frameCount;
	float frameDuration;
	float stateTime;
	PlayMode playMode;
};