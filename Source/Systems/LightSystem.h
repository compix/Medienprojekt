#pragma once
#include <ecstasy/systems/IteratingSystem.h>
#include <SFML/Graphics/RenderTexture.hpp>

namespace sf{
	class RenderWindow;
}

class LightSystem : public IteratingSystem<LightSystem>
{
public:
	LightSystem();
	void processEntity(Entity *entity, float deltaTime) override;

	//inline const sf::Texture* getLightMap() { return &m_lightMap.getTexture(); };
private:
	//sf::RenderTexture m_lightMap;
};