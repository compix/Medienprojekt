#include "RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <iostream>
#include <format.h>
#include "../Components/ParticleComponent.h"
#include "../Components/ShaderComponent.h"
#include "../GameGlobals.h"
#include "../Events/ExitEvent.h"
#include "../Components/RenderOffsetComponent.h"
#include "../Components/JumpComponent.h"
#include "../GameConstants.h"

RenderSystem::RenderSystem(LayerManager* layerManager)
	: m_layerManager(layerManager), m_fpsCalculator(200, 100, 16)
{
	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		GameGlobals::events->emit<ExitEvent>();
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

//	updateAndShowFrameStats((float)dt);
}

void RenderSystem::render(EntityLayer* layer)
{
	EntityGrid grid = layer->getEntityGrid();

	for (int y = 0; y < layer->getHeight(); y++)
	{
		for (int x = 0; x < layer->getWidth(); x++)
		{
			layer->sort(DepthComparator(), x, y);
			EntityCollection& collection = grid[x][y];

			for (auto& e : collection)
			{
				if (!e.valid())
					continue;

				auto transform = e.component<TransformComponent>();
				auto sprite = e.component<SpriteComponent>();
				auto particleComponent = e.component<ParticleComponent>();

				if (!transform)
					continue;

				if (sprite)
				{
					if (e.has_component<RenderOffsetComponent>() && !e.component<RenderOffsetComponent>()->scheduledForRemoval)
					{
						auto offset = e.component<RenderOffsetComponent>();

						if (e.has_component<JumpComponent>())
						{
							auto jumpComp = e.component<JumpComponent>();

							sprite->sprite.setPosition(	jumpComp->fromX * GameConstants::CELL_WIDTH  + GameConstants::CELL_WIDTH  / 2.f + offset->xOffset, 
														jumpComp->fromY * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f + offset->yOffset);
						} 
						else
						{
							sprite->sprite.setPosition(transform->x + offset->xOffset, transform->y + offset->yOffset);
						}
					}
					else
					{
						sprite->sprite.setPosition(transform->x, transform->y);
					}

					//Entfernt die Komponente erst nachdem die Letzte Position gesetzt wurde, verhindert das hin und her springen des Sprite
					if (e.has_component<RenderOffsetComponent>() && e.component<RenderOffsetComponent>()->scheduledForRemoval)		
						e.remove<RenderOffsetComponent>();

					sprite->sprite.setRotation(transform->rotation);
					sprite->sprite.setScale(transform->scaleX, transform->scaleY);
					GameGlobals::window->draw(sprite->sprite);
				}

				if (particleComponent && particleComponent->emitter->alive())
					GameGlobals::window->draw(*particleComponent->emitter);
			}
		}
	}
}

void RenderSystem::updateAndShowFrameStats(float deltaTime)
{
	/*
	m_fpsCalculator.addFrame();
	m_fpsText.setString(fmt::format("{:.1f} FPS", m_fpsCalculator.getFps()));
	float w = m_fpsText.getLocalBounds().width;
	float x = 790 - w;
	m_fpsText.setPosition(x, 0);
	GameGlobals::window->draw(m_fpsText);
	*/

	static float timeElapsed = 0.0f;
	static uint16_t frameCount = 0;

	timeElapsed += deltaTime;
	++frameCount;

	if (timeElapsed >= 1.f)
	{
		std::wostringstream outs;
		outs.precision(6);

		uint16_t fps = static_cast<uint16_t>(frameCount / timeElapsed);
		float mspf = 1000.0f / fps; // ms per frame

		outs << L"FPS: " << fps << L"    "
			 << L"Frame Time: " << mspf << L" ms";

		GameGlobals::window->setTitle(outs.str());
		timeElapsed = 0.f;
		frameCount = 0;
	}
	
}
