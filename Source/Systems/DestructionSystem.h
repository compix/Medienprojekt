#pragma once
#include <ecstasy/core/EntitySystem.h>



class DestructionSystem : public EntitySystem<DestructionSystem>
{
public:
	void update(float dt) override;
};
