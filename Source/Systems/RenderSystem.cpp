#include "RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <iostream>

RenderSystem::RenderSystem(sf::RenderWindow* pWindow, LayerManager* pLayerManager)
	: m_pWindow(pWindow), m_pLayerManager(pLayerManager) {}

void RenderSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	for (auto& layer : m_pLayerManager->getLayers())
	{
		render(layer.second.get());
	}
}

void RenderSystem::render(EntityLayer* layer)
{
	EntityGrid grid = layer->getEntityGrid();

	for (int y = 0; y < layer->getHeight(); y++)
	{
		for (int x = 0; x < layer->getWidth(); x++)
		{
			layer->sort(DepthComparator(), x, y);
			EntityCollection collection = grid[x][y];

			for (auto& e : collection)
			{
				ComponentHandle<TransformComponent> transform = e.component<TransformComponent>();
				ComponentHandle<SpriteComponent> sprite = e.component<SpriteComponent>();

				if (!transform || !sprite)
					continue;

				sprite->sprite.setPosition(transform->x, transform->y);
				sprite->sprite.setRotation(transform->rotation);
				sprite->sprite.setScale(transform->scaleX, transform->scaleY);

				m_pWindow->draw(sprite->sprite);
			}
		}
	}
}


