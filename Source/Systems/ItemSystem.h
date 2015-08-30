#pragma once
#include <entityx/System.h>
#include <unordered_map>
#include "../Components/ItemComponent.h"
#include "../Utils/Common.h"

struct BombLandedOnEntityEvent;
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
	void receive(const BombLandedOnEntityEvent& bombLandedOnEntityEvent);
	
	static bool needsItem(entityx::Entity& entity, ItemType itemType);
private:
	void dropItemOnCell(LevelCell from, LevelCell to, ItemType itemType);
	void getFreeCells(std::vector<LevelCell>& outFreeCells);

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

	static std::unordered_map<ItemType, uint8_t, EnumClassHash> m_maxItemCounts;
	static std::unordered_map<ItemType, uint8_t, EnumClassHash> m_minItemCounts;
};

