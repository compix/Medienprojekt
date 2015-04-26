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

using namespace std;

int main()
{
	TextureLoader textureLoader;
	textureLoader.loadAllFromJson("assets/json/textures.json");
	Texture tex = textureLoader.get("char_idle");

	sf::Sprite sprite;
	//sprite.setColor(sf::Color(0, 255, 0));
	sprite.setTexture(tex);

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

	Game game(&window);

	// idle entity
	entityx::Entity entity = game.entities.create();
	TransformComponent transformComponent;
	transformComponent.scaleX = 0.5f;
	transformComponent.scaleY = 0.5f;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	AnimationComponent animationComponent;
	animationComponent.colCount = 8;
	animationComponent.rowCount = 3;
	animationComponent.frameCount = 20;
	animationComponent.frameDuration = 0.025f;
	animationComponent.playMode = PlayMode::LOOP;

	entity.assign<AnimationComponent>(animationComponent);

	// dieing entity
	sf::Sprite deathSprite;
	//sprite.setColor(sf::Color(0, 255, 0));
	Texture tex2 = textureLoader.get("char_death");
	deathSprite.setTexture(tex2);

	entityx::Entity dieingEntity = game.entities.create();

	TransformComponent transformComponent2;
	transformComponent2.x = 150.f;
	transformComponent2.y = 150.f;
	transformComponent2.scaleX = 0.5f;
	transformComponent2.scaleY = 0.5f;
	dieingEntity.assign<TransformComponent>(transformComponent2);
	dieingEntity.assign<SpriteComponent>(deathSprite);

	AnimationComponent animationComponent2;
	animationComponent2.colCount = 8;
	animationComponent2.rowCount = 5;
	animationComponent2.frameCount = 40;
	animationComponent2.frameDuration = 0.05f;
	animationComponent2.playMode = PlayMode::LOOP_PING_PONG;

	dieingEntity.assign<AnimationComponent>(animationComponent2);

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