#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>

using namespace entityx;

class Game : public EntityX
{
public:
	Game(sf::RenderWindow* pWindow);
	~Game();

	void update(TimeDelta dt);
};

