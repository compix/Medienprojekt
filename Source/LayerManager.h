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
public:
	EntityLayer* createLayer(int width, int height, int layer);
	~LayerManager();

	inline LayerContainer& getLayers() { return m_layers; }
	inline EntityLayer* getLayer(int layer) { assert(m_layers.count(layer) > 0); return m_layers[layer].get(); }

	void configure(entityx::EventManager& events);
	void receive(const entityx::EntityDestroyedEvent& e);

	void add(Entity entity);
	void remove(Entity entity);
	void update();

	EntityCollection getEntities(int layer, int cellX, int cellY);

	template<class T>
	bool hasEntityWithComponent(int layer, int cellX, int cellY);

	template<class C1, class C2, class... Args>
	bool hasEntityWithComponents(int layer, int cellX, int cellY);

	template<class C>
	bool hasEntityWithComponents(int layer, int cellX, int cellY);

	//inline bool hasEntityWithComponents(int layer, int cellX, int cellY) { return true; }

	template<class C1, class C2, class... Args>
	bool hasComponents(Entity& e);

	template<class C>
	inline bool hasComponents(Entity& e) { return e.has_component<C>(); }

	//inline bool hasComponents(Entity& e) { return true; }

	// An invalid entity will be returned if there is no such entity
	template<class T>
	Entity getEntityWithComponent(int layer, int cellX, int cellY);
	bool isFree(int layer, int cellX, int cellY);
private:
	LayerContainer m_layers;
};

template <class T>
bool LayerManager::hasEntityWithComponent(int layer, int cellX, int cellY)
{
	for (auto& e : getEntities(layer, cellX, cellY))
		if (e.has_component<T>())
			return true;

	return false;
}

template <class C1, class C2, class ... Args>
bool LayerManager::hasEntityWithComponents(int layer, int cellX, int cellY)
{
	for (auto& e : getEntities(layer, cellX, cellY))
		if (hasComponents<C1, C2, Args...>(e))
			return true;

	return false;
}

template <class C>
bool LayerManager::hasEntityWithComponents(int layer, int cellX, int cellY)
{
	for (auto& e : getEntities(layer, cellX, cellY))
		if (e.has_component<C>())
			return true;

	return false;
}

template <class C1, class C2, class ... Args>
bool LayerManager::hasComponents(Entity& e)
{
	return e.has_component<C1>() && hasComponents<C2, Args...>(e);
}

template <class T>
Entity LayerManager::getEntityWithComponent(int layer, int cellX, int cellY)
{
	for (auto& e : getEntities(layer, cellX, cellY))
		if (e.has_component<T>())
			return e;

	return Entity();
}