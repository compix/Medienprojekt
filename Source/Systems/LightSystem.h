#pragma once
#include <entityx/System.h>
#include <SFML/Graphics/RenderTexture.hpp>

namespace sf{
	class RenderWindow;
}

class LightSystem : public entityx::System<LightSystem>
{
public:
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	//inline const sf::Texture* getLightMap() { return &m_lightMap.getTexture(); };
private:
	//sf::RenderTexture m_lightMap;
};