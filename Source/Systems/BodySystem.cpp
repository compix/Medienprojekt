#include "BodySystem.h"
#include "../Components/BodyComponent.h"
#include "../Components/TransformComponent.h"
#include <PhysixSystem.h>
#include <Components/SpriteComponent.h>

using namespace entityx;

void BodySystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<BodyComponent> body;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<SpriteComponent> sprite;
	
	float scale = PhysixSystem::m_Scale;

	for (Entity entity : entityManager.entities_with_components(body, transform, sprite))
	{

		transform->x = (body->body->GetPosition().x*scale);
		transform->y = (body->body->GetPosition().y*scale);

	}
}