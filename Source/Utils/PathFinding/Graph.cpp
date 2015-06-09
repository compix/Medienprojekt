#include "Graph.h"
#include "../../LayerManager.h"
#include "../../GameConstants.h"

Graph::Graph(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
	//auto mainLayer = m_layerManager->getLayer(GameConstants::MAIN_LAYER);
	//m_nodes.resize(mainLayer->getWidth()*mainLayer->getHeight());
}