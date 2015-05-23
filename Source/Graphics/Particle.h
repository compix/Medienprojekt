#pragma once
#include <SFML/Graphics.hpp>

struct Particle
{
	sf::Vector2f pos;
	sf::Vector2f velocity;
	sf::Vector2f size;
	sf::Color color;
	float lifetime; // in seconds
	float rotation; // in radians

	float angle; // (direction angle) in radians
};