#pragma once
#include <entityx/Event.h>
#include <entityx/System.h>
#include "../LayerManager.h"

class LayerManager;

class BlinkSystem : public entityx::System<BlinkSystem>
{
public:
	BlinkSystem(LayerManager* layerManager);
	~BlinkSystem();
	void configure(entityx::EventManager &event_manager) override;
	void update(EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
private:
	LayerManager* m_layerManager;

};