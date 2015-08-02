#include "BodySystem.h"
#include "../Components/BodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../PhysixSystem.h"
#include "../Components/SpriteComponent.h"
#include "../GameGlobals.h"
#include "../EntityFactory.h"

void BodySystem::update(float dt)
{
	ComponentHandle<BodyComponent> body;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<SpriteComponent> sprite;
	
	float scale = PhysixSystem::m_Scale;

	for (Entity *entity : entityManager.entities_with_components(body, transform, sprite))
	{
		transform->x = (PhysixSystem::toWorld(body->body->GetPosition().x));
		transform->y = (PhysixSystem::toWorld(body->body->GetPosition().y));
	}
}