#pragma once
#include <entityx/System.h>

struct DeathEvent;

class DestructionSystem : public entityx::System<DestructionSystem>
{
public:
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
};
