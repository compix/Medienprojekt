#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>
#include <signal11/Signal.h>

class InventorySystem : public EntitySystem<InventorySystem>
{
public:
	~InventorySystem();

	void addedToEngine(Engine *engine) override;

	void update(float dt) override;

private:
	void onBombExploded(Entity *bomb);
	ConnectionScope m_connections;
};

