#pragma once
#include <SFML/Graphics.hpp>
#include "../EntityLayer.h"
#include "../LayerManager.h"
#include "../Utils/FpsCalculator.h"
#include <ecstasy/core/EntitySystem.h>

class RenderSystem : public EntitySystem<RenderSystem>
{
public:
	RenderSystem(LayerManager* layerManager);

	void update(float dt) override;
private:
	void render(EntityLayer* layer);
	void updateAndShowFrameStats(float deltaTime);

private:
	LayerManager* m_layerManager;
	sf::Font m_font;
	sf::Text m_fpsText;
	FpsCalculator m_fpsCalculator;
};

