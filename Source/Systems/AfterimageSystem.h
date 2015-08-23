#pragma once
#include <entityx/Event.h>
#include <entityx/System.h>
#include "../Events/TimeoutEvent.h"

class LayerManager;

class AfterimageSystem : public entityx::System<AfterimageSystem>, public entityx::Receiver<AfterimageSystem>
{
public:
	AfterimageSystem();
	~AfterimageSystem();
	void configure(entityx::EventManager &event_manager) override;
	void receive(const TimeoutEvent& timeoutEvent);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

};