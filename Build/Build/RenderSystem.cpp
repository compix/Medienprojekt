#include "RenderSystem.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"

RenderSystem::RenderSystem(sf::RenderWindow* pWindow)
	: m_pWindow(pWindow) {}

void RenderSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	ComponentHandle<TransformComponent> transform;
	ComponentHandle<SpriteComponent> sprite;

	for (Entity entity : entityManager.entities_with_components(transform, sprite))
	{
		sprite->sprite.setPosition(transform->x, transform->y);
		sprite->sprite.setRotation(transform->rotation);
		sprite->sprite.setScale(transform->scaleX, transform->scaleY);

		m_pWindow->draw(sprite->sprite);
	}
}


