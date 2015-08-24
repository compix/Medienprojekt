#include "PunchBomb.h"
#include "../../Components/InputComponent.h"
#include "../../Components/AIComponent.h"
#include "../../Components/DirectionComponent.h"
#include "../../Components/CellComponent.h"

bool PunchBomb::operator()(entityx::Entity& entity)
{
	assert(entity.valid() && entity.has_component<InputComponent>());
	assert(entity.has_component<AIComponent>() && entity.has_component<CellComponent>());

	auto aiComponent = entity.component<AIComponent>();
	auto inputComponent = entity.component<InputComponent>();
	auto node = aiComponent->behaviorNode;
	if (node)
	{
		auto directionComponent = entity.component<DirectionComponent>();
		auto cell = entity.component<CellComponent>();
		assert(directionComponent);
		directionComponent->direction = CommonUtil::toDirection(abs(cell->x - node->x), abs(cell->y - node->y));
		inputComponent->skillButtonPressed = true;
	}

	return true;
}