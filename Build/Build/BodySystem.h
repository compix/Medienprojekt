#pragma once
#include <entityx/entityx.h>

using namespace entityx;

class BodySystem : public System<BodySystem>
{
public:
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
};