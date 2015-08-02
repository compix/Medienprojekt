#pragma once
#include <ecstasy/core/Component.h>
#include <SFML/Graphics.hpp>

struct SpriteComponent: public ECS::Component<SpriteComponent>
{
	sf::Sprite sprite;
};