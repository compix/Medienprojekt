#pragma once
#include <ecstasy/core/Component.h>
#include "../Utils/Common.h"

struct DirectionComponent: public ECS::Component<DirectionComponent>
{
	Direction direction = Direction::DOWN;
	
	void reset() override {
		direction = Direction::DOWN;
	}
};