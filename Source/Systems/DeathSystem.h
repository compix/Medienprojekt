#pragma once
#include <ecstasy/core/EntitySystem.h>



class DeathSystem : public EntitySystem<DeathSystem>
{
public:
	~DeathSystem();
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;

private:
	void onDeath(Entity *dyingEntity);
};
