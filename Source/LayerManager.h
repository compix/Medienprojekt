#pragma once
#include "EntityLayer.h"
#include <memory>
#include <map>

using std::vector;

typedef std::shared_ptr<EntityLayer> EntityLayerPtr;
typedef std::map<int, EntityLayerPtr> LayerContainer;

class LayerManager : public entityx::Receiver<LayerManager>
{
public:
	EntityLayer* createLayer(int width, int height, int layer);
	~LayerManager();

	inline LayerContainer& getLayers() { return m_layers; }

	void configure(entityx::EventManager& events);
	void receive(const entityx::EntityDestroyedEvent& e);

	void add(Entity entity);
	void remove(Entity entity);
	void update();

	EntityCollection getEntities(int layer, int cellX, int cellY);
	bool hasSolidBlock(int layer, int cellX, int cellY);
	bool isFree(int layer, int cellX, int cellY);

private:
	LayerContainer m_layers;
};
