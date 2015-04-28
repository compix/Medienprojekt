#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <iostream>
#include "json/json.h"
#include <fstream>
#include "TextureLoader.h"
#include "Game.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "AnimationComponent.h"
#include "EntityLayer.h"

using namespace std;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

	Game game(&window);

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Time deltaTime = clock.restart();

		window.clear();
		game.update(deltaTime.asSeconds());
		window.display();
	}

	return 0;
}