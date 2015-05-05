#include "RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <iostream>
#include <stdio.h>

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
		if (!layer.isStatic())
			layer.sort(DepthComparator());

		render(layer);
	}

	showFPS();
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

#ifndef WIN32
	#define _snprintf snprintf
#endif

void RenderSystem::showFPS()
{
	m_fpsCalculator.addFrame();
	char buffer [20];
	_snprintf(buffer, 20, "%.1f FPS", m_fpsCalculator.getFps());
	m_fpsText.setString(buffer);
	float w = m_fpsText.getLocalBounds().width;
	float x = m_pWindow->getSize().x - w;
	m_fpsText.setPosition(x, 0);
	m_pWindow->draw(m_fpsText);
}
