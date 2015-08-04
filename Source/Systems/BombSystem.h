#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>
#include <signal11/Signal.h>

class BombSystem : public EntitySystem<BombSystem>
{
public:
	BombSystem();
	~BombSystem();
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;

private:
	void onTimeout(Entity *affectedEntity);
	void onEntityGotHit(Entity *damageDealer, Entity *damagedEntity, int damage);
	void detonate(Entity *entity);

private:
	ConnectionScope m_connections;
};

