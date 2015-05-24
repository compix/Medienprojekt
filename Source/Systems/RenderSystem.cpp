#include "RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <iostream>
#include <format.h>
#include "../Components/ShaderComponent.h"

RenderSystem::RenderSystem(sf::RenderWindow* window, LayerManager* layerManager)
	: m_window(window), m_layerManager(layerManager), m_fpsCalculator(200, 100, 16)
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
	for (auto& layer : m_layerManager->getLayers())
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

				auto shaderComponent = e.component<ShaderComponent>();
				sf::Shader* shader = nullptr;

				if (shaderComponent)
				{
					switch (shaderComponent->type)
					{
					case ShaderType::SFML: 
						break;
					case ShaderType::LIGHTMAP:
						// Not sure if it'll be used.
						break;
					default: 
						break;
					}
				}

				sprite->sprite.setPosition(transform->x, transform->y);
				sprite->sprite.setRotation(transform->rotation);
				sprite->sprite.setScale(transform->scaleX, transform->scaleY);

				m_window->draw(sprite->sprite, shader);
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
	m_window->draw(m_fpsText);
}
