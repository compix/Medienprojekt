#pragma once
#include "Particle.h"
#include <vector>
#include <SFML/Graphics.hpp>

const float GRAVITY = 9.81f;

class ParticleEmitter : public sf::Drawable, public sf::Transformable
{
public:
	ParticleEmitter();
	ParticleEmitter(int maxParticles, sf::Vector2f pos);

	void update(float deltaTime);

	inline void setTexture(sf::Texture texture) { m_texture = texture; }

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	void spawnParticle();
	void updateQuad(int vertexStart, Particle& p);

private:
	sf::Vector2f m_pos;

	int m_maxParticles;
	int m_numActive;
	std::vector<Particle> m_particles;
	std::vector<sf::Vertex> m_vertices;
	sf::Texture m_texture;

	float m_spawnTime; // in seconds: A new particle will spawn after that time if there is a free slot for it
	float m_spawnTimeRemaining;

	sf::Vector2f m_startVelocity;
	sf::Vector2f m_startSize;

	float m_maxLifetime;

	float m_gravityModifier;
};