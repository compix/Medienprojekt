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
    const float m_blinkTime;
	float m_remaining;
	bool blinked;
};
