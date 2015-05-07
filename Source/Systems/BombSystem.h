#pragma once
#include <entityx/System.h>

struct EntityGotHitEvent;
class EntityFactory;
struct TimeoutEvent;

class BombSystem : public entityx::System<BombSystem>, public entityx::Receiver<BombSystem>
{
public:
	BombSystem(EntityFactory* entityFactory);

	void configure(entityx::EventManager& events) override;
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const TimeoutEvent& timeoutEvent);
	void receive(const EntityGotHitEvent& entityGotHitEvent);

private:
	void detonate(entityx::Entity entity);

private:
	EntityFactory* m_entityFactory;
};

