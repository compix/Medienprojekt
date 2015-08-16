#include "PlaceBomb.h"
#include "../../Components/InputComponent.h"

void PlaceBomb::update(entityx::Entity& entity)
{
	assert(entity.valid() && entity.has_component<InputComponent>());

	auto inputComponent = entity.component<InputComponent>();
	inputComponent->bombButtonPressed = true;
}