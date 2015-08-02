#pragma once
#include <ecstasy/core/EntitySystem.h>


class EntityFactory;


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

private:
	void detonate(Entity *entity);
};

