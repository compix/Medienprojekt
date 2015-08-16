#include "UseSkill.h"
#include "../../Components/InputComponent.h"

void UseSkill::operator()(entityx::Entity& entity)
{
	assert(entity.valid() && entity.has_component<InputComponent>());

	auto inputComponent = entity.component<InputComponent>();
	inputComponent->skillButtonPressed = true;
}