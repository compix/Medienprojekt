#pragma once
#include "../Components/BodyComponent.h"
#include <ecstasy/systems/IteratingSystem.h>

class BodySystem : public IteratingSystem<BodySystem>
{
public:
	BodySystem();
	
	void processEntity(Entity *entity, float deltaTime) override;
};