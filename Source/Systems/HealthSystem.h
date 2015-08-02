#pragma once
#include <ecstasy/core/EntitySystem.h>



class HealthSystem : public EntitySystem<HealthSystem>
{
public:
	~HealthSystem();
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;

private:
	void onEntityGotHit(Entity *damageDealer, Entity *damagedEntity, int damage);
};

