#pragma once
#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>
#include "EntityLayer.h"
#include "LayerManager.h"

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class RenderSystem : public System<RenderSystem>
{
public:
	RenderSystem(sf::RenderWindow* pWindow, LayerManager* pLayerManager);

	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;

private:
	void render(EntityLayer& layer);

private:
	sf::RenderWindow* m_pWindow;
	LayerManager* m_pLayerManager;
};

