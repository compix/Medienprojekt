#pragma once
#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>

using namespace entityx;

class RenderSystem : public System<RenderSystem>
{
public:
	RenderSystem(sf::RenderWindow* pWindow);

	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;

private:
	sf::RenderWindow* m_pWindow;
};

