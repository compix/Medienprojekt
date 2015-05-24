#pragma once
#include <entityx/System.h>
#include "../EntityFactory.h"

class DamageSystem : public entityx::System<DamageSystem>
{
public:
	DamageSystem(LayerManager* layerManager);

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

private:
	LayerManager* m_layerManager;
};