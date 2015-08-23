#pragma once
#include <entityx/System.h>

struct ItemPickedUpEvent;
class LayerManager;

class ItemSystem : public entityx::System<ItemSystem>, public entityx::Receiver<ItemSystem>
{
public:
	ItemSystem(LayerManager* layerManager);
	~ItemSystem();

	void configure(entityx::EventManager& events) override;

	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const entityx::EntityDestroyedEvent& destroyedEvent);
	void receive(const ItemPickedUpEvent& pickedUpEvent);

private:
	LayerManager* m_layerManager;
};

