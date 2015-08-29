#pragma once
#include <entityx/Entity.h>
#include "../../LayerManager.h"

class ThrowBomb
{
public:
	explicit ThrowBomb(LayerManager* layerManager);

	bool operator()(entityx::Entity& entity);

private:
	LayerManager* m_layerManager;
};