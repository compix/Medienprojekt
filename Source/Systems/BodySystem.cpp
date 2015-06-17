#include "BodySystem.h"
#include "../Components/BodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../PhysixSystem.h"
#include "../Components/SpriteComponent.h"
#include "../GameGlobals.h"

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
	const BodyComponent* body = event.component.get();
	Entity entity = event.entity;

	body->body->SetUserData(static_cast<void*>(&entity));
	std::cout << entity.id() << std::endl;
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
	}
}

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