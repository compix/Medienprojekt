#pragma once
#include <SFML/Graphics.hpp>

class ParticleEmitter;

struct Particle
{
	sf::Vector2f pos; // relative position to the emitter
	float lifetime; // in seconds
	float rotation; // in radians
	float directionAngle; // in radians
	bool goalReached;
};