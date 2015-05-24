#include "ParticleEmitter.h"
#include <Utils/Random.h>
#include "Utils/Colors.h"
#include <Utils/Math.h>
#include "ParticleManager.h"
#include <iostream>

ParticleEmitter::ParticleEmitter()
	:ParticleEmitter(sf::Vector2f(0.f, 0.f))
{
}

ParticleEmitter::ParticleEmitter(sf::Vector2f pos)
	: m_pos(pos), m_gravityModifier(1.f), m_speedModifier(1.f), m_scheduledForRemoval(false), m_spawnWidth(0.f), m_spawnHeight(0.f), m_rotation(0.f)
{
	spawnTime(0.1f);
	burstTime(1.f);

	burstParticleNumber(0);

	m_startSize = sf::Vector2f(24.f, 24.f);

	m_maxLifetime = 5.f;
	m_velocityFunction = [](float t) { return sf::Vector2f(t*50.f, t*50.f); };
	m_angularVelocityFunction = [](float t) { return 0.f; };
	m_sizeFunction = [](float t) { return sf::Vector2f(24.f, 24.f); };
	m_colorFunction = [](float t) { return RGB(255, 255, 255); };
	m_transparencyFunction = [](float t) { return 255 - t * 255; };
}

void ParticleEmitter::update(float deltaTime)
{
	m_spawnTimeRemaining -= deltaTime;
	m_burstTimeRemaining -= deltaTime;

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

	if (m_burstTimeRemaining <= 0.f)
	{
		for (int i = 0; i < m_burstParticleNumber; i++)
			spawnParticle();

		m_burstTimeRemaining = m_burstTime;
	}
}

void ParticleEmitter::update(Particle& p, float deltaTime, sf::Color& colorOut, sf::Vector2f& sizeOut)
{
	float t = (m_maxLifetime - p.lifetime) / m_maxLifetime; // normalized time: [0, 1]
	sf::Vector2f velocity = m_velocityFunction(t);

	Math::rotate(velocity, p.angle);

	p.rotation += m_angularVelocityFunction(t);

	velocity *= m_speedModifier;
	velocity.y += PARTICLE_GRAVITY * m_gravityModifier;

	p.pos += velocity * deltaTime;

	sizeOut = m_sizeFunction(t);
	auto rgb = m_colorFunction(t);

	colorOut.r = rgb.r;
	colorOut.g = rgb.g;
	colorOut.b = rgb.b;
	colorOut.a = m_transparencyFunction(t);
}

ParticleEmitter& ParticleEmitter::spawnTime(float spawnTime)
{
	m_spawnTime = spawnTime;
	m_spawnTimeRemaining = spawnTime;
	return *this;
}

ParticleEmitter& ParticleEmitter::burstTime(float burstTime)
{
	m_burstTime = burstTime;
	m_burstTimeRemaining = burstTime;
	return *this;
}

ParticleEmitter& ParticleEmitter::burstParticleNumber(int num)
{
	m_burstParticleNumber = num;
	return *this;
}

void ParticleEmitter::spawnParticle()
{
	if (m_particleManager->hasSpace())
	{
		// Init
		Particle* particle = m_particleManager->spawnParticle();
		float angle = Random::getFloat(0.f, Math::PI*2.f);
		particle->rotation = 0.f;
		particle->lifetime = m_maxLifetime;

		float x = Random::getFloat(-m_spawnWidth*0.5f, m_spawnWidth*0.5f);
		float y = Random::getFloat(-m_spawnHeight*0.5f, m_spawnHeight*0.5f);

		particle->pos = sf::Vector2f(x, y);
		particle->angle = angle;
		particle->emitter = this;
	}
}