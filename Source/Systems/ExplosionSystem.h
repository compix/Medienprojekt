#pragma once
#include <ecstasy/core/EntitySystem.h>
#include "../EntityFactory.h"

class ExplosionSystem : public EntitySystem<ExplosionSystem>
{
public:
	explicit ExplosionSystem(LayerManager* layerManager);

	void update(float dt) override;

private:
	LayerManager* m_layerManager;
};