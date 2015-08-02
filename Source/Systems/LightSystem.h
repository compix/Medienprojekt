#pragma once
#include <ecstasy/core/EntitySystem.h>
#include <SFML/Graphics/RenderTexture.hpp>

namespace sf{
	class RenderWindow;
}

class LightSystem : public EntitySystem<LightSystem>
{
public:
	void update(float dt) override;

	//inline const sf::Texture* getLightMap() { return &m_lightMap.getTexture(); };
private:
	//sf::RenderTexture m_lightMap;
};