#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/systems/IteratingSystem.h>
#include <signal11/Signal.h>

class LayerManager;

class ItemSystem : public IteratingSystem<ItemSystem>
{
public:
	ItemSystem(LayerManager* layerManager);
	~ItemSystem();

	void addedToEngine(Engine *engine) override;

	void processEntity(Entity *entity, float deltaTime) override;

private:
	void onEntityDestroyed(Entity *entity);
	void onItemPickedUp(Entity *item, Entity *itemReceiver);

private:
	LayerManager* m_layerManager;
	ConnectionScope m_connections;
};

