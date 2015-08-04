#pragma once
#include <ecstasy/systems/IteratingSystem.h>
#include "../EntityFactory.h"

class InputHandleSystem : public IteratingSystem<InputHandleSystem>
{
public:
	InputHandleSystem();
	
	void processEntity(Entity *entity, float deltaTime) override;
};

