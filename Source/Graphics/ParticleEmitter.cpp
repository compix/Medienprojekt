#include "ParticleEmitter.h"
#include <iostream>
#include <Utils/Random.h>

ParticleEmitter::ParticleEmitter()
	:ParticleEmitter(1000, sf::Vector2f(0.f, 0.f))
{
}

ParticleEmitter::ParticleEmitter(int maxParticles, sf::Vector2f pos)
	: m_maxParticles(maxParticles), m_pos(pos), m_numActive(0), m_gravityModifier(1.f)
{
	m_particles.resize(m_maxParticles);
	m_vertices.resize(m_maxParticles * 4);

	m_spawnTime = 0.01f;
	m_spawnTimeRemaining = m_spawnTime;

	m_startSize = sf::Vector2f(24.f, 24.f);
	m_startVelocity = sf::Vector2f(100.f, 0.f);

	m_maxLifetime = 5.f;
}

void ParticleEmitter::update(float deltaTime)
{
	int i = 0;
	while (i < m_numActive)
	{
		Particle& p = m_particles[i];
		
		p.lifetime -= deltaTime;

		if (p.lifetime <= 0.f)
		{
			m_numActive--;
			m_particles[i] = m_particles[m_numActive]; // Place the last active particle at the position of the dead particle
			continue;
		}

		// Update the particle
		sf::Vector2f velocity = p.velocity;
		velocity.y += GRAVITY * m_gravityModifier;

		p.pos += velocity * deltaTime;

		updateQuad(i * 4, p);

		i++;
	}

	m_spawnTimeRemaining -= deltaTime;
	if (m_spawnTimeRemaining <= 0.f)
	{
		m_spawnTimeRemaining = m_spawnTime;
		spawnParticle();
	}
}

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.shader = nullptr;
	states.blendMode = sf::BlendAdd;

	states.texture = &m_texture;

	if (m_numActive > 0)
		target.draw(&m_vertices[0], m_numActive * 4, sf::Quads, states);
}

void ParticleEmitter::updateQuad(int vertexStart, Particle& p)
{
	float hw = p.size.x*0.5f;
	float hh = p.size.y*0.5f;
	float x = p.pos.x;
	float y = p.pos.y;
	float texWidth = m_texture.getSize().x;
	float texHeight = m_texture.getSize().y;

	m_vertices[vertexStart].position = sf::Vector2f(x-hw, y-hh);
	m_vertices[vertexStart].texCoords = sf::Vector2f(0.f, 0.f);
	m_vertices[vertexStart].color.a = (p.lifetime / m_maxLifetime) * 255;

	m_vertices[vertexStart + 1].position = sf::Vector2f(x+hw, y-hh);
	m_vertices[vertexStart + 1].texCoords = sf::Vector2f(texWidth, 0.f);
	m_vertices[vertexStart + 1].color.a = (p.lifetime / m_maxLifetime) * 255;

	m_vertices[vertexStart + 2].position = sf::Vector2f(x+hw, y+hh);
	m_vertices[vertexStart + 2].texCoords = sf::Vector2f(texWidth, texHeight);
	m_vertices[vertexStart + 2].color.a = (p.lifetime / m_maxLifetime) * 255;

	m_vertices[vertexStart + 3].position = sf::Vector2f(x-hw, y+hh);
	m_vertices[vertexStart + 3].texCoords = sf::Vector2f(0.f, texHeight);
	m_vertices[vertexStart + 3].color.a = (p.lifetime / m_maxLifetime) * 255;
}

void ParticleEmitter::spawnParticle()
{
	if (m_numActive < m_maxParticles)
	{
		// Init
		Particle& particle = m_particles[m_numActive];
		particle.size = m_startSize;

		const float PI = 3.14159265359f;

		float angle = Random::getFloat(0.f, PI*2.f);
		particle.velocity = sf::Vector2f(cosf(angle)*50.f, sinf(angle)*50.f);
		particle.lifetime = m_maxLifetime;
		particle.pos = m_pos;

		m_numActive++;
	}
}