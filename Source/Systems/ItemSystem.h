#pragma once
#include <entityx/System.h>
#include <unordered_map>
#include "../Components/ItemComponent.h"
#include "../Utils/Common.h"

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
	/**
	* Returns true if successfully removed.
	*/
	bool removeItem(entityx::Entity& entity, ItemType itemType);
	/**
	* Returns true if successfully added.
	*/
	bool addItem(entityx::Entity& entity, ItemType itemType);

private:
	LayerManager* m_layerManager;

	std::unordered_map<ItemType, uint8_t, EnumClassHash> m_maxItemCounts;
	std::unordered_map<ItemType, uint8_t, EnumClassHash> m_minItemCounts;
};

