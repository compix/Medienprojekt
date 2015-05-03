#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <iostream>
#include "json/json.h"
#include <fstream>
#include "Utils/TextureLoader.h"
#include "Game.h"
#include "Menu.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "EntityLayer.h"
#include "Utils/InputManager.h"
#include "SFMLDebugDraw.h"

using namespace std;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
	window.setKeyRepeatEnabled(false);

	InputManager inputManager;

	SFMLDebugDraw debugDraw(window);

	debugDraw.SetFlags(b2Draw::e_shapeBit);
	debugDraw.AppendFlags(b2Draw::e_jointBit);
	debugDraw.AppendFlags(b2Draw::e_centerOfMassBit);
	debugDraw.AppendFlags(b2Draw::e_aabbBit);

	Game game(&window, inputManager, &debugDraw);

	Menu menu(window);

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (!menu.handleEvent(event))
				inputManager.handleEvent(event);
		}

		sf::Time deltaTime = clock.restart();

		window.clear();
		game.update(deltaTime.asSeconds());
		//menu.draw();
		window.display();
	}

	return 0;
}