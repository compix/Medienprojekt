#pragma once
#include <SFML/Graphics.hpp>

struct SpriteComponent
{
	SpriteComponent() {}
	SpriteComponent(const sf::Sprite& sprite) : sprite(sprite) {}

	sf::Sprite sprite;
};