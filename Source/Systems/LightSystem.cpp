#include "LightSystem.h"
#include "../Components/LightComponent.h"
#include "../GameGlobals.h"

LightSystem::LightSystem() 
: IteratingSystem(Family::all<LightComponent>().get()) {}

void LightSystem::processEntity(Entity *entity, float deltaTime)
{
	GameGlobals::window->draw(entity->get<LightComponent>()->light);
}