#pragma once
#include <SFML/System/Vector2.hpp>

struct Particle
{
	sf::Vector2f pos;
	sf::Vector2f velocity;
	sf::Vector2f size;
	float lifetime; // in seconds
};