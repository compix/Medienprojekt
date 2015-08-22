#pragma once
#include <entityx/System.h>
#include "../Events/HoldingEvent.h"
#include "../Events/ThrowBombEvent.h"

class HoldingSystem : public entityx::System<HoldingSystem>, public entityx::Receiver<HoldingSystem>
{
public:
	~HoldingSystem();
	void configure(entityx::EventManager& events) override;
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const HoldingEvent& holdEvent);
	void receive(const ThrowBombEvent& throwEvent);
};

