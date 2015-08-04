#pragma once
#include <ecstasy/systems/IteratingSystem.h>
#include "../EntityFactory.h"

class DamageSystem : public IteratingSystem<DamageSystem>
{
public:
	DamageSystem(LayerManager* layerManager);

	void processEntity(Entity *damageDealer, float deltaTime) override;

private:
	LayerManager* m_layerManager;
};