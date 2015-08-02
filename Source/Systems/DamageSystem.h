#pragma once
#include <ecstasy/core/EntitySystem.h>
#include "../EntityFactory.h"

class DamageSystem : public EntitySystem<DamageSystem>
{
public:
	DamageSystem(LayerManager* layerManager);

	void update(float dt) override;

private:
	LayerManager* m_layerManager;
};