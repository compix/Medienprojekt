#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <iostream>
#include "json/json.h"
#include <fstream>
#include "TextureLoader.h"

using namespace std;

int main()
{
	TextureLoader textureLoader;
	textureLoader.load("destructable_block.png", "block");

	Texture tex = textureLoader.get("block");

	entityx::EntityX entityX;
	entityx::Entity entity = entityX.entities.create();

	std::cout << entity << std::endl;
	
	// json
	Json::Value root;
	Json::Reader reader;
	ifstream test("assets/json/images.json", ifstream::binary);
	
	if (!reader.parse(test, root, false))
		cout << "Parsing error." << endl;

	std::string crosshair = root.get("crosshair", "nothing").asString();
	for (auto it = root.begin(); it != root.end(); it++)
	{
		
		cout << it.key().asString() << endl;
	}

	cout << root.size() << endl;
	cout << crosshair << endl;

	sf::Sprite sprite;
	sprite.setColor(sf::Color(0, 255, 0));
	sprite.setTexture(tex);

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
		window.draw(sprite);
		window.display();
	}

	return 0;
}