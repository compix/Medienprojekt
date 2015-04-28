#include "RenderSystem.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include <iostream>

RenderSystem::RenderSystem(sf::RenderWindow* pWindow, LayerManager* pLayerManager)
	: m_pWindow(pWindow), m_pLayerManager(pLayerManager) {}

void RenderSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	for (auto& layer : m_pLayerManager->getLayers())
	{
		if (!layer.isStatic())
			layer.sort(DepthComparator());

		render(layer);
	}
}

void RenderSystem::render(EntityLayer& layer)
{
	for (auto e : layer.getEntities())
	{
		ComponentHandle<TransformComponent> transform = e.component<TransformComponent>();
		ComponentHandle<SpriteComponent> sprite = e.component<SpriteComponent>();

		sprite->sprite.setPosition(transform->x, transform->y);
		sprite->sprite.setRotation(transform->rotation);
		sprite->sprite.setScale(transform->scaleX, transform->scaleY);

		m_pWindow->draw(sprite->sprite);
	}
}


