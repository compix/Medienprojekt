#pragma once
#include <ecstasy/core/EntitySystem.h>
using namespace ECS;



class InventorySystem : public EntitySystem<InventorySystem>
{
public:
	~InventorySystem();

	void addedToEngine(Engine *engine) override;

	void update(float dt) override;

private:
	void onBombExploded(Entity *bomb);
};

