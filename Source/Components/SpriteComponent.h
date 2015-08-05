#pragma once
#include <ecstasy/core/Component.h>
#include <SFML/Graphics.hpp>

struct SpriteComponent: public Component<SpriteComponent>
{
	SpriteComponent() {}
	SpriteComponent(const sf::Sprite& sprite) : sprite(sprite) {}

	sf::Sprite sprite;
};