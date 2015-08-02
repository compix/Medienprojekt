#pragma once
#include "../EntityLayer.h"

using namespace ECS;

class InputSystem : public EntitySystem<InputSystem>
{
public:
	void update(float dt) override;
};

