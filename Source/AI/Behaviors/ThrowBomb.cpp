#include "ThrowBomb.h"
#include "../../Components/InputComponent.h"
#include "../../Components/AIComponent.h"
#include "../../Components/DirectionComponent.h"
#include "../../Components/CellComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/BombComponent.h"

ThrowBomb::ThrowBomb(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
}

bool ThrowBomb::operator()(entityx::Entity& entity)
{
	assert(m_layerManager);
	assert(entity.valid() && entity.has_component<InputComponent>());
	assert(entity.has_component<AIComponent>() && entity.has_component<CellComponent>());

	auto aiComponent = entity.component<AIComponent>();
	auto inputComponent = entity.component<InputComponent>();
	auto inventory = entity.component<InventoryComponent>();
	auto cell = entity.component<CellComponent>();

	if (!inventory->isHoldingBomb)
		return true;

	auto behaviorNode = aiComponent->behaviorNode;
	if (behaviorNode)
	{
		auto directionComponent = entity.component<DirectionComponent>();
		
		assert(directionComponent);
		int dx = behaviorNode->x - cell->x;
		int dy = behaviorNode->y - cell->y;

		if (abs(dx) + abs(dy) == 0 || abs(dx) + abs(dy) > 1)
			return true;

		directionComponent->direction = CommonUtil::toDirection(dx, dy);
		inputComponent->bombButtonPressed = true;
	}

	return true;
}