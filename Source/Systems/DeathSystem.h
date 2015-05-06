#pragma once
#include <entityx/System.h>

struct DeathEvent;

class DeathSystem : public entityx::System<DeathSystem>, public entityx::Receiver<DeathSystem>
{
public:
	void configure(entityx::EventManager& events) override;
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const DeathEvent& deathEvent);
};
