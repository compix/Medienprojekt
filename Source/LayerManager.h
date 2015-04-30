#pragma once
#include "EntityLayer.h"
#include <vector>
#include <memory>

using namespace std;

class LayerManager
{
public:
	EntityLayer& createLayer(int layer, bool _static);

	vector<EntityLayer>& getLayers() { return m_layers; }

	template<class T>
	void sortLayers(T comparator);
private:
	void sort();
private:
	vector<EntityLayer> m_layers;
};

template<class T>
void LayerManager::sortLayers(T comparator)
{
	for (auto& layer : m_layers)
		layer.sort(comparator);
}

