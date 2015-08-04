#include "BodySystem.h"
#include "../Components/BodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../PhysixSystem.h"
#include "../Components/SpriteComponent.h"
#include "../GameGlobals.h"
#include "../EntityFactory.h"

BodySystem::BodySystem() : IteratingSystem(Family::all<BodyComponent, TransformComponent, SpriteComponent>().get()) {
	
}

void BodySystem::processEntity(Entity *entity, float deltaTime)
{
	float scale = PhysixSystem::m_Scale;
	auto body = entity->get<BodyComponent>();
	auto transform = entity->get<TransformComponent>();
	transform->x = (PhysixSystem::toWorld(body->body->GetPosition().x));
	transform->y = (PhysixSystem::toWorld(body->body->GetPosition().y));
}