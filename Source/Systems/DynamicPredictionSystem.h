#pragma once
#include <entityx/System.h>

class DynamicPredictionSystem : public entityx::System<DynamicPredictionSystem>
{
public:
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
};
