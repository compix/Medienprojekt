#pragma once
#include <entityx/entityx.h>
#include "../Components/BodyComponent.h"

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;
using entityx::Receiver;
using entityx::ComponentAddedEvent;

class BodySystem : public System<BodySystem>, public Receiver<BodySystem>
{
public:
	void configure(entityx::EventManager &event_manager) override;
	void receive(const entityx::ComponentAddedEvent<BodyComponent> &event);
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	

	

};