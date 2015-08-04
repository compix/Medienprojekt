#pragma once
#include <ecstasy/systems/IteratingSystem.h>
#include "../EntityFactory.h"

class ExplosionSystem : public IteratingSystem<ExplosionSystem>
{
public:
	explicit ExplosionSystem(LayerManager* layerManager);

	void processEntity(Entity *entity, float deltaTime) override;

private:
	LayerManager* m_layerManager;
};