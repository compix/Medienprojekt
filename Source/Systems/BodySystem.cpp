#include "BodySystem.h"
#include "../Components/BodyComponent.h"
#include "../Components/TransformComponent.h"
#include <PhysixSystem.h>

void BodySystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<BodyComponent> body;
	ComponentHandle<TransformComponent> transform;

	for (Entity entity : entityManager.entities_with_components(body, transform))
	{
		transform->x = body->body->GetPosition().x*PhysixSystem::m_Scale;
		transform->y = body->body->GetPosition().y*PhysixSystem::m_Scale;
	}
}