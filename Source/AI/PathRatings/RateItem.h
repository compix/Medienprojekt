#pragma once
#include <cinttypes>
#include "../PathFinding/PathEngine.h"

struct GraphNode;
struct AIPath;

class RateItem
{
public:
	explicit RateItem(LayerManager* layerManager);

	bool operator()(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity);

private:
	LayerManager* m_layerManager;
};