#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <iostream>

int main()
{
	entityx::EntityX entityX;
	entityx::Entity entity = entityX.entities.create();

	std::cout << entity << std::endl;

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}