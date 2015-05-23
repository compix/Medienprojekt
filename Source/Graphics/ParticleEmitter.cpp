#include "ParticleEmitter.h"
#include <iostream>
#include <Utils/Random.h>
#include <Utils/Common.h>

ParticleEmitter::ParticleEmitter()
	:ParticleEmitter(100, sf::Vector2f(0.f, 0.f))
{
}

ParticleEmitter::ParticleEmitter(int maxParticles, sf::Vector2f pos)
	: m_maxParticles(maxParticles), m_pos(pos), m_numActive(0), m_gravityModifier(1.f), m_speedModifier(1.f)
{
	m_particles.resize(m_maxParticles);
	m_vertices.resize(m_maxParticles * 4);

	m_spawnTime = 0.1f;
	m_spawnTimeRemaining = m_spawnTime;

	m_startSize = sf::Vector2f(24.f, 24.f);

	m_maxLifetime = 5.f;
	m_velocityFunction = [](float t) { t = (t + 1) / 2.f; return sf::Vector2f(t*50.f, t*50.f); };
	m_angularVelocityFunction = [](float t) { return 0.f; };
	m_sizeFunction = [](float t) { return sf::Vector2f(24.f, 24.f); };
	m_colorFunction = [](float t) { return sf::Color(255, 255, 255, 255 - t*255); };
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
		float t = (m_maxLifetime - p.lifetime) / m_maxLifetime * 2.f - 1.f; // normalized time: [-1, 1]
		sf::Vector2f velocity = m_velocityFunction(t);

		Math::rotate(velocity, p.angle);
		
		p.rotation += m_angularVelocityFunction(t);

		velocity *= m_speedModifier;
		velocity.y += GRAVITY * m_gravityModifier;

		p.pos += velocity * deltaTime;

		p.size = m_sizeFunction(t);
		p.color = m_colorFunction((t+1.f)/2.f); // t should be in range [0,1] for colors

		updateQuad(i * 4, p);

		i++;
	}

	m_spawnTimeRemaining -= deltaTime;

	// If the spawn Time is negative then more than one Particle CAN be spawned
	if (m_spawnTimeRemaining <= 0.f)
	{
		do
		{
			spawnParticle();
			m_spawnTimeRemaining += m_spawnTime;
		} while (m_spawnTimeRemaining <= 0.f);	

		m_spawnTimeRemaining = m_spawnTime;
	}
}

ParticleEmitter& ParticleEmitter::maxParticles(int maxParticles)
{
	m_maxParticles = maxParticles;
	m_particles.resize(m_maxParticles);
	m_vertices.resize(m_maxParticles * 4);
	return *this;
}

ParticleEmitter& ParticleEmitter::spawnTime(float spawnTime)
{
	m_spawnTime = spawnTime;
	m_spawnTimeRemaining = spawnTime;
	return *this;
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
	float texWidth = (float)m_texture.getSize().x;
	float texHeight = (float)m_texture.getSize().y;

	m_vertices[vertexStart + 0].position = sf::Vector2f(-hw, -hh);
	m_vertices[vertexStart + 1].position = sf::Vector2f(hw, -hh);
	m_vertices[vertexStart + 2].position = sf::Vector2f(hw, hh);
	m_vertices[vertexStart + 3].position = sf::Vector2f(-hw, hh);

	// Rotate around the center of the Particle
	Math::rotate(m_vertices[vertexStart + 0].position, p.rotation);
	Math::rotate(m_vertices[vertexStart + 1].position, p.rotation);
	Math::rotate(m_vertices[vertexStart + 2].position, p.rotation);
	Math::rotate(m_vertices[vertexStart + 3].position, p.rotation);
	
	m_vertices[vertexStart + 0].texCoords = sf::Vector2f(0.f, 0.f);
	m_vertices[vertexStart + 1].texCoords = sf::Vector2f(texWidth, 0.f);
	m_vertices[vertexStart + 2].texCoords = sf::Vector2f(texWidth, texHeight);
	m_vertices[vertexStart + 3].texCoords = sf::Vector2f(0.f, texHeight);

	// Make the alpha value linearly decrease to 0 based on lifetime
	m_vertices[vertexStart + 0].color = p.color;
	m_vertices[vertexStart + 1].color = p.color;
	m_vertices[vertexStart + 2].color = p.color;
	m_vertices[vertexStart + 3].color = p.color;

	// Translate the vertices
	for (int i = 0; i < 4; i++)
	{
		m_vertices[vertexStart + i].position.x += x;
		m_vertices[vertexStart + i].position.y += y;
	}
}

void ParticleEmitter::spawnParticle()
{
	if (m_numActive < m_maxParticles)
	{
		// Init
		Particle& particle = m_particles[m_numActive];
		particle.size = m_startSize;
		float angle = Random::getFloat(0.f, Math::PI*2.f);
		particle.velocity = sf::Vector2f(0.f, 0.f);
		particle.rotation = 0.f;
		particle.lifetime = m_maxLifetime;
		particle.pos = m_pos;
		particle.angle = angle;

		m_numActive++;
	}
}