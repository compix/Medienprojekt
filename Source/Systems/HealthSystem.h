#pragma once
#include <entityx/System.h>

struct EntityGotHitEvent;

class HealthSystem : public entityx::System<HealthSystem>, public entityx::Receiver<HealthSystem>
{
public:
	void configure(entityx::EventManager& events) override;
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const EntityGotHitEvent& entityGotHit);

private:
	entityx::EventManager* m_eventManager;
};

