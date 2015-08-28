#include "BodySystem.h"
#include "../Components/BodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../PhysixSystem.h"
#include "../Components/SpriteComponent.h"
#include "../GameGlobals.h"
#include "../EntityFactory.h"
#include "../Components/InventoryComponent.h"
#include "../BodyFactory.h"
#include "../Components/DynamicComponent.h"

using namespace entityx;

BodySystem::~BodySystem()
{
	GameGlobals::events->unsubscribe<ComponentAddedEvent<BodyComponent>>(*this);
	GameGlobals::events->unsubscribe<EntityDestroyedEvent>(*this);
}

void BodySystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<ComponentAddedEvent<BodyComponent>>(*this);
	event_manager.subscribe<EntityDestroyedEvent>(*this);
}

void BodySystem::receive(const entityx::ComponentAddedEvent<BodyComponent>& event)
{

}

void BodySystem::receive(const entityx::EntityDestroyedEvent& event)
{
	auto entity = event.entity;

	if (!entity.valid())
		return;

	if (entity.has_component<BodyComponent>())
	{
		auto body = entity.component<BodyComponent>()->body;
		body->GetWorld()->DestroyBody(body);
		BodyFactory::freeEntityId(entity.id().id());
	}

	//GameGlobals::entityFactory->destroyEntity(entity);
}

void BodySystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<BodyComponent> body;
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<SpriteComponent> sprite;
	
	float scale = PhysixSystem::m_Scale;

	for (Entity entity : entityManager.entities_with_components(body, transform, sprite))
	{
		transform->x = (PhysixSystem::toWorld(body->body->GetPosition().x));
		transform->y = (PhysixSystem::toWorld(body->body->GetPosition().y));
		auto dynamic = entity.component<DynamicComponent>();
		if (dynamic.valid())
		{
			auto v = body->body->GetLinearVelocity();
			dynamic->velX = PhysixSystem::toWorld(v.x);
			dynamic->velY = PhysixSystem::toWorld(v.y);
		}
	}
}