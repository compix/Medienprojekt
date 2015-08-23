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
	EntityLayer* createLayer(uint8_t width, uint8_t height, int layer);
	~LayerManager();
	void reset();

	inline LayerContainer& getLayers() { return m_layers; }
	inline EntityLayer* getLayer(int layer) { assert(m_layers.count(layer) > 0); return m_layers[layer].get(); }

	void configure(entityx::EventManager& events);
	void receive(const entityx::EntityDestroyedEvent& e);

	void add(Entity entity);
	void remove(Entity entity);
	void update();

	inline bool inBounds(int layer, uint8_t cellX, uint8_t cellY)
	{
		assert(m_layers.count(layer));
		return m_layers[layer]->inBounds(cellX, cellY);
	}

	/**
	* A copy is returned because adding entities during iteration is viable. A reference would cause an exception in this case.
	*/
	EntityCollection getEntities(int layer, uint8_t cellX, uint8_t cellY);

	template<class C>
	bool hasEntityWithComponent(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* True is returned if an entity in the given layer and cell with all of the specified components exists.
	*/
	template<class C1, class... Args>
	bool hasEntityWithComponents(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* True is returned if the entity has all of the specified components.
	*/
	template<class C1, class C2, class... Args>
	bool hasComponents(Entity& e);

	/**
	* True is returned if the entity has AT LEAST one of the specified components.
	*/
	template<class C>
	inline bool hasComponents(Entity& e) { return e.valid() && e.has_component<C>(); }

	/**
	* Returns the first entity in the given layer and cell if it has all of the specified components.
	* An invalid entity will be returned if there is no such entity
	*/
	template<class C1, class... Args>
	Entity getEntityWithComponents(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* Returns the first entity in the given layer and cell if it has the specified component.
	* An invalid entity will be returned if there is no such entity
	*/
	template <class C>
	Entity getEntityWithComponent(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* True is returned if the entity has AT LEAST one of the specified components.
	*/
	template<class C1, class C2, class... Args>
	bool hasOneComponent(Entity& e);

	/**
	* True is returned if the entity has the specified component.
	*/
	template<class C1>
	inline bool hasOneComponent(Entity& e) { assert(e); return e.has_component<C1>(); }

	/**
	* True is returned if an entity in the given layer and cell with AT LEAST on of the specified components exists.
	*/
	template<class C1, class... Args>
	bool hasEntityWithOneComponent(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* Returns the first entity in the given layer and cell if it has AT LEAST one of the specified components.
	* An invalid entity will be returned if there is no such entity
	*/
	template<class C1, class... Args>
	Entity getEntityWithOneComponent(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* Returns all entities in the given layer and cell with all of the specified components.
	*/
	template<class C1, class... Args>
	std::vector<Entity> getEntitiesWithComponents(int layer, uint8_t cellX, uint8_t cellY);

	/**
	* Returns all entities in the given layer and cell with at least one of the specified components.
	*/
	template<class C1, class... Args>
	std::vector<Entity> getEntitiesWithOneComponent(int layer, uint8_t cellX, uint8_t cellY);

	bool isInLayer(int layer, entityx::Entity& entity);

	bool isFree(int layer, uint8_t cellX, uint8_t cellY);

	void updateCell(entityx::Entity& entity);
private:
	LayerContainer m_layers;
};

template <class C>
bool LayerManager::hasEntityWithComponent(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (e.valid() && e.has_component<C>())
			return true;

	return false;
}

template <class C1, class ... Args>
bool LayerManager::hasEntityWithComponents(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasComponents<C1, Args...>(e))
			return true;

	return false;
}

template <class C1, class C2, class ... Args>
bool LayerManager::hasComponents(Entity& e)
{
	assert(e);
	return e.has_component<C1>() && hasComponents<C2, Args...>(e);
}

template <class C1, class C2, class ... Args>
bool LayerManager::hasOneComponent(Entity& e)
{
	assert(e);
	return e.has_component<C1>() || hasOneComponent<C2, Args...>(e);
}

template <class C>
Entity LayerManager::getEntityWithComponent(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (e.valid() && e.has_component<C>())
			return e;

	return Entity();
}

template <class C1, class ... Args>
Entity LayerManager::getEntityWithComponents(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasComponents<C1, Args...>(e))
			return e;

	return Entity();
}

template <class C1, class ... Args>
bool LayerManager::hasEntityWithOneComponent(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasOneComponent<C1, Args...>(e))
			return true;

	return false;
}

template <class C1, class ... Args>
Entity LayerManager::getEntityWithOneComponent(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasOneComponent<C1, Args...>(e))
			return e;

	return Entity();
}

template <class C1, class ... Args>
std::vector<Entity> LayerManager::getEntitiesWithComponents(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	std::vector<Entity> entities;
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasComponents<C1, Args...>(e))
			entities.push_back(e);

	return entities;
}

template <class C1, class ... Args>
std::vector<Entity> LayerManager::getEntitiesWithOneComponent(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	std::vector<Entity> entities;
	for (auto& e : m_layers[layer]->get(cellX, cellY))
		if (hasOneComponent<C1, Args...>(e))
			entities.push_back(e);

	return entities;
}