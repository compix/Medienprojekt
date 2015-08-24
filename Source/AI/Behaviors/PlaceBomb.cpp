#include "PlaceBomb.h"
#include "../../Components/InputComponent.h"

bool PlaceBomb::operator()(entityx::Entity& entity)
{
	assert(entity.valid() && entity.has_component<InputComponent>());

	auto inputComponent = entity.component<InputComponent>();
	inputComponent->bombButtonPressed = true;
	return true;
}