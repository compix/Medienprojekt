#pragma once
#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>
#include "../EntityLayer.h"
#include "../LayerManager.h"
#include "../Utils/FpsCalculator.h"

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class RenderSystem : public System<RenderSystem>
{
public:
	RenderSystem(LayerManager* layerManager);

	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
private:
	void render(EntityLayer* layer);
	void updateAndShowFrameStats(float deltaTime);

private:
	LayerManager* m_layerManager;
	sf::Font m_font;
	sf::Text m_fpsText;
	FpsCalculator m_fpsCalculator;
};

