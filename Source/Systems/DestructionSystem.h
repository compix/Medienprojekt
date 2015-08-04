#pragma once
#include <ecstasy/systems/IteratingSystem.h>

class DestructionSystem : public IteratingSystem<DestructionSystem>
{
public:
	DestructionSystem();
	
	void processEntity(Entity *entity, float deltaTime) override;
};
