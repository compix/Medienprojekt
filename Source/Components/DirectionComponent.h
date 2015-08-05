#pragma once
#include <ecstasy/core/Component.h>
#include "../Utils/Common.h"

struct DirectionComponent: public Component<DirectionComponent>
{
	DirectionComponent() : direction(Direction::DOWN) {}
	DirectionComponent(Direction dir) : direction(dir) {}

	Direction direction;
};