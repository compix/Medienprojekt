#pragma once
#include <entityx/System.h>
#include "../EntityFactory.h"

struct ExplosionSpreadRequest
{
	ExplosionSpreadRequest(uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime)
		:x(x), y(y), direction(direction), range(range), spreadTime(spreadTime) {}

	uint8_t x, y;
	Direction direction;
	uint8_t range;
	float spreadTime;
};

class ExplosionSystem : public entityx::System<ExplosionSystem>
{
public:
	explicit ExplosionSystem(LayerManager* layerManager);

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

private:
	LayerManager* m_layerManager;
};