#pragma once
#include <entityx/System.h>
#include <EntityFactory.h>

class ExplosionSystem : public entityx::System<ExplosionSystem>
{
public:
	ExplosionSystem(EntityFactory* entityFactory, LayerManager* layerManager);

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

private:
	EntityFactory* m_entityFactory;
	LayerManager* m_layerManager;
};