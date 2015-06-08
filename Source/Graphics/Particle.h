#pragma once
#include <SFML/Graphics.hpp>

class ParticleEmitter;

struct Particle
{
	sf::Vector2f pos;
	float lifetime; // in seconds
	float rotation; // in radians
	float angle; // (direction angle) in radians
	bool goalReached;
};