#include "BodySystem.h"
#include "BodyComponent.h"
#include "TransformComponent.h";

void BodySystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<BodyComponent> body;
	ComponentHandle<TransformComponent> transform;

	for (Entity entity : entityManager.entities_with_components(body, transform))
	{
		transform->x = body->body->GetPosition().x;
		transform->y = body->body->GetPosition().y;
	}
}