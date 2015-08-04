#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>
#include <signal11/Signal.h>

class DeathSystem : public EntitySystem<DeathSystem>
{
public:
	~DeathSystem();
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;

private:
	void onDeath(Entity *dyingEntity);
	ConnectionScope m_connections;
};
