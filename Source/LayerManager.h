#pragma once
#include "EntityLayer.h"
#include <memory>
#include <map>
#include <type_traits>

using std::vector;

class Graph;

typedef std::shared_ptr<EntityLayer> EntityLayerPtr;
typedef std::map<int, EntityLayerPtr> LayerContainer;

class LayerManager : public entityx::Receiver<LayerManager>
{
	struct EntityUpdateInfo
	{
		EntityUpdateInfo(entityx::Entity& entity, uint8_t x, uint8_t y, int layer)
			:entity(entity), x(x), y(y), layer(layer) {}

		entityx::Entity entity;
		uint8_t x;
		uint8_t y;
		int layer;
	};
public:
	EntityLayer* createLayer(int width, int height, int layer);
	~LayerManager();
	void reset();

	inline LayerContainer& getLayers() { return m_layers; }
	inline EntityLayer* getLayer(int layer) { assert(m_layers.count(layer) > 0); return m_layers[layer].get(); }

	void configure(entityx::EventManager& events);
	void receive(const entityx::EntityDestroyedEvent& e);

	void add(Entity entity);
	void remove(Entity entity);
	void update();

	inline bool inBounds(int layer, int cellX, int cellY) { return m_layers[layer]->inBounds(cellX, cellY); }

	/**
	* A copy is returned because adding entities during iteration is viable. A reference would cause an exception in this case.
	*/
	EntityCollection getEntities(int layer, int cellX, int cellY);

	template<class C>
	bool hasEntityWithComponent(int layer, int cellX, int cellY);

	template<class C1, class C2, class... Args>
	bool hasEntityWithComponents(int layer, int cellX, int cellY);

	template<class C>
	bool hasEntityWithComponents(int layer, int cellX, int cellY);

	template<class C1, class C2, class... Args>
	bool hasComponents(Entity& e);

	template<class C>
	inline bool hasComponents(Entity& e) { return e.valid() && e.has_component<C>(); }

	// An invalid entity will be returned if there is no such entity
	template<class C>
	Entity getEntityWithComponent(int layer, int cellX, int cellY);

	// An invalid entity will be returned if there is no such entity
	template<class C1, class C2, class... Args>
	Entity getEntityWithComponents(int layer, int cellX, int cellY);

	bool isFree(int layer, int cellX, int cellY);

	void updateCell(entityx::Entity& entity);
private:
	LayerContainer m_layers;

	std::vector<EntityUpdateInfo> m_scheduledForRemoval;
	std::vector<EntityUpdateInfo> m_scheduledForPush;
};

template <class C>
bool LayerManager::hasEntityWithComponent(int layer, int cellX, int cellY)
{
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (e.valid() && e.has_component<C>())
			return true;

	return false;
}

template <class C1, class C2, class ... Args>
bool LayerManager::hasEntityWithComponents(int layer, int cellX, int cellY)
{
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasComponents<C1, C2, Args...>(e))
			return true;

	return false;
}

template <class C>
bool LayerManager::hasEntityWithComponents(int layer, int cellX, int cellY)
{
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (e.valid() && e.has_component<C>())
			return true;

	return false;
}

template <class C1, class C2, class ... Args>
bool LayerManager::hasComponents(Entity& e)
{
	return e.valid() && e.has_component<C1>() && hasComponents<C2, Args...>(e);
}

template <class C>
Entity LayerManager::getEntityWithComponent(int layer, int cellX, int cellY)
{
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (e.valid() && e.has_component<C>())
			return e;

	return Entity();
}

template <class C1, class C2, class ... Args>
Entity LayerManager::getEntityWithComponents(int layer, int cellX, int cellY)
{
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasComponents<C1, C2, Args...>(e))
			return e;

	return Entity();
}