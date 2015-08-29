#pragma once
#include "Action.h"

/**
* To avoid that AI blinks to often this action adds a time restriction.
*/
class BlinkAction : public Action
{
public:
	BlinkAction(PathEngine* pathEngine, PathRating pathRating, Behavior behavior, LayerManager* layerManager);
	void update(entityx::Entity& entity, float deltaTime) override;

private:
	bool isBlinkReady(entityx::Entity& entity, float deltaTime);

private:
	float m_remaining;
	const float m_blinkTime;
	bool blinked;
};