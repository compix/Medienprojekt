#pragma once
#include <ecstasy/core/EntitySystem.h>
#include "../EntityFactory.h"

class InputHandleSystem : public EntitySystem<InputHandleSystem>
{
public:
	void update(float dt) override;
};

