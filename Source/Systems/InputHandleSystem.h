#pragma once
#include <entityx/System.h>
#include "../EntityFactory.h"

class InputHandleSystem : public entityx::System<InputHandleSystem>
{
public:
	explicit InputHandleSystem(LayerManager* layerManager);

	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

private:
	bool canPlacePortal(entityx::Entity& entity);
	bool canPunch(entityx::Entity& entity);
private:
	LayerManager* m_layerManager;
};

