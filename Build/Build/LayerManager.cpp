#include "LayerManager.h"
#include <algorithm>

EntityLayer& LayerManager::createLayer(int layer, bool _static)
{
	m_layers.push_back(EntityLayer(layer, _static));
	EntityLayer& entityLayer = m_layers.at(m_layers.size() - 1);
	sort();

	return entityLayer;
}

void LayerManager::sort()
{
	auto comparator = [](const EntityLayer& layer1, const EntityLayer& layer2) { return layer1.getValue() < layer2.getValue(); };
	std::sort(m_layers.begin(), m_layers.end(), comparator);
}
