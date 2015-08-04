#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>
#include <signal11/Signal.h>

class HealthSystem : public EntitySystem<HealthSystem>
{
public:
	~HealthSystem();
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;

private:
	void onEntityGotHit(Entity *damageDealer, Entity *damagedEntity, int damage);
	ConnectionScope m_connections;
};

