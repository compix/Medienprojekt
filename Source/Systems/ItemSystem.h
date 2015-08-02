#pragma once
#include <ecstasy/core/EntitySystem.h>


class LayerManager;

class ItemSystem : public EntitySystem<ItemSystem>
{
public:
	ItemSystem(LayerManager* layerManager);
	~ItemSystem();

	void addedToEngine(Engine *engine) override;

	void update(float dt) override;

private:
	void onEntityDestroyed(Entity *entity);
	void onItemPickedUp(Entity *item, Entity *itemReceiver);

private:
	LayerManager* m_layerManager;
};

