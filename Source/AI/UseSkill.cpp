#include "../Components/InputComponent.h"
#include "UseSkill.h"

void UseSkill::update(entityx::Entity& entity)
{
	assert(entity.valid() && entity.has_component<InputComponent>());

	auto inputComponent = entity.component<InputComponent>();
	inputComponent->skillButtonPressed = true;
}