#pragma once
#include "EntityLayer.h"
#include <memory>
#include <map>
#include <ecstasy/core/Engine.h>
#include <signal11/Signal.h>

using std::vector;

class Graph;

typedef std::shared_ptr<EntityLayer> EntityLayerPtr;
typedef std::map<int, EntityLayerPtr> LayerContainer;

class LayerManager 
{
public:
	EntityLayer* createLayer(int width, int height, int layer);
	~LayerManager();

	inline LayerContainer& getLayers() { return m_layers; }
	inline EntityLayer* getLayer(int layer) { assert(m_layers.count(layer) > 0); return m_layers[layer].get(); }

	void addedToEngine(Engine *engine);
	void removedFromEngine(Engine *engine);

	void add(Entity *entity);
	void remove(Entity *entity);
	void update();

	EntityCollection getEntities(int layer, int cellX, int cellY);

	template<class T>
	bool hasEntityWithComponent(int layer, int cellX, int cellY);

	// An invalid entity will be returned if there is no such entity
	template<class T>
	Entity *getEntityWithComponent(int layer, int cellX, int cellY);
	bool isFree(int layer, int cellX, int cellY);
	
private:
	void onEntityRemoved(Entity *entity);
	
private:
	LayerContainer m_layers;
	ConnectionScope m_connections;
	const std::vector<Entity *> *m_dynamicEntities;
};

template <class T>
bool LayerManager::hasEntityWithComponent(int layer, int cellX, int cellY)
{
	for (auto *e : getEntities(layer, cellX, cellY))
		if (e->has<T>())
			return true;

	return false;
}

template <class T>
Entity *LayerManager::getEntityWithComponent(int layer, int cellX, int cellY)
{
	for (auto *e : getEntities(layer, cellX, cellY))
		if (e->has<T>())
			return e;

	return nullptr;
}