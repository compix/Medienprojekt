#include "RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <iostream>
#include <format.h>

RenderSystem::RenderSystem(sf::RenderWindow* pWindow, LayerManager* pLayerManager)
	: m_pWindow(pWindow), m_pLayerManager(pLayerManager), m_fpsCalculator(200, 100, 16)
{
	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		exit(EXIT_FAILURE);
	}

	m_fpsText.setFont(m_font);
	m_fpsText.setCharacterSize(24);
	m_fpsText.setColor(sf::Color::White);
	m_fpsText.setStyle(sf::Text::Bold);
}

void RenderSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	for (auto& layer : m_pLayerManager->getLayers())
	{
		render(layer.second.get());
	}

	showFPS();
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

void RenderSystem::showFPS()
{
	m_fpsCalculator.addFrame();
	m_fpsText.setString(fmt::format("{:.1f} FPS", m_fpsCalculator.getFps()));
	float w = m_fpsText.getLocalBounds().width;
	float x = 790 - w;
	m_fpsText.setPosition(x, 0);
	m_pWindow->draw(m_fpsText);
}
