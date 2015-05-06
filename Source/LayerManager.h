#pragma once
#include "EntityLayer.h"
#include <memory>
#include <map>

using std::vector;

typedef std::shared_ptr<EntityLayer> EntityLayerPtr;
typedef std::map<int, EntityLayerPtr> LayerContainer;

class LayerManager : public Receiver<LayerManager>
{
public:
	EntityLayer* createLayer(int width, int height, int layer);

	inline LayerContainer& getLayers() { return m_layers; }

	void configure(entityx::EventManager& events);
	void receive(const EntityDestroyedEvent& e);

	void addToLayer(int layer, Entity entity);
	void removeFromLayer(int layer, Entity entity);
	void remove(Entity entity);
	void update();

	EntityCollection& getEntities(int layer, int cellX, int cellY);
	bool hasSolidBlock(int layer, int cellX, int cellY);
private:
	LayerContainer m_layers;
};
