#include "UseDirectionSkill.h"
#include "../../Components/InputComponent.h"
#include "../../Components/AIComponent.h"
#include "../../Components/DirectionComponent.h"
#include "../../Components/CellComponent.h"

bool UseDirectionSkill::operator()(entityx::Entity& entity)
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
		int dx = node->x - cell->x;
		int dy = node->y - cell->y;

		if (abs(dx) + abs(dy) == 0 || abs(dx) + abs(dy) > 1)
			return true;

		directionComponent->direction = CommonUtil::toDirection(dx, dy);
		inputComponent->skillButtonPressed = true;
	}

	return true;
}