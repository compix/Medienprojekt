#pragma once
#include "../EntityLayer.h"
#include <ecstasy/systems/IteratingSystem.h>

class InputSystem : public IteratingSystem<InputSystem>
{
public:
	InputSystem();
	void processEntity(Entity *entity, float deltaTime) override;
};

