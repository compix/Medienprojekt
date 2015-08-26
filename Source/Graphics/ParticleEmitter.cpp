#include "ParticleEmitter.h"
#include "../Utils/Random.h"
#include "../Utils/Colors.h"
#include "../Utils/Math.h"
#include <float.h>
#include "../Components/TransformComponent.h"

ParticleEmitter::ParticleEmitter()
	:ParticleEmitter(500)
{
}

ParticleEmitter::ParticleEmitter(uint32_t maxParticles)
	: m_cachedTime(-FLT_MAX)
{
	m_particles.resize(maxParticles);
	m_vertices.resize(maxParticles * 4);

	refresh();
}

void ParticleEmitter::refresh()
{
	m_numActive = 0;
	m_scheduledForRemoval = false;

	m_lastPos.x = 0.f;
	m_lastPos.y = 0.f;

	m_following = false;
	m_followSpeed = 15.f;
	goalRadius(100.f);
	burstParticleNumber(0);
	position(0.f, 0.f);
	gravityModifier(1.f);
	speedModifier(1.f);
	spawnWidth(0.f);
	spawnHeight(0.f);
	rotation(0.f);
	spawnTime(0.1f);
	burstTime(1.f);
	burstNumber(-1);
	blendMode(sf::BlendAdd);
	spawnDuration(FLT_MAX);
	
	m_startSize = sf::Vector2f(24.f, 24.f);

	m_maxLifetime = 5.f;
	m_velocityFunction = [](float t) { return sf::Vector2f(t*50.f, t*50.f); };
	m_angularVelocityFunction = [](float t) { return 0.f; };
	m_sizeFunction = [](float t) { return sf::Vector2f(24.f, 24.f); };
	m_colorFunction = [](float t) { return RGB(255, 255, 255); };
	m_transparencyFunction = [](float t) { return 255 - t * 255; };
}

ParticleEmitter& ParticleEmitter::follow(entityx::Entity entity)
{
	m_target = entity;
	assert(m_target.valid() && m_target.has_component<TransformComponent>());
	auto transform = m_target.component<TransformComponent>();
	m_following = true;
	m_pos.x = transform->x;
	m_pos.y = transform->y;
	m_lastPos = m_pos;
	return *this;
}

void ParticleEmitter::update(float deltaTime)
{
	m_spawnTimeRemaining -= deltaTime;
	m_burstTimeRemaining -= deltaTime;
	m_spawnDuration -= deltaTime;

	
	// Spawn particles
	if (m_spawnDuration > 0.f)
	{
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

		if ((m_burstNumber < 0 || m_burstNumber > 0) && m_burstTimeRemaining <= 0.f)
		{
			for (int i = 0; i < m_burstParticleNumber; i++)
				spawnParticle();

			m_burstTimeRemaining = m_burstTime;
			m_burstNumber = m_burstNumber < 0 ? -1 : m_burstNumber - 1;
		}
	}

	// Update particles
	sf::Color color;
	sf::Vector2f size;
	uint32_t i = 0;
	while (i < m_numActive)
	{
		Particle& p = m_particles[i];

		p.lifetime -= deltaTime;
		if (p.lifetime <= 0.f || m_scheduledForRemoval)
		{
			m_numActive--;

			m_particles[i] = m_particles[m_numActive]; // Place the last active particle at the position of the dead particle
			continue;
		}

		update(p, deltaTime);
		updateQuad(i * 4, p);

		i++;
	}

	m_lastPos = m_pos;
}

void ParticleEmitter::update(Particle& p, float deltaTime)
{
	float t = (m_maxLifetime - p.lifetime) / m_maxLifetime; // normalized time: [0, 1]
	if (!Math::nearEq(t, m_cachedTime))
	{
		m_cachedTime = t;
		m_cachedVelocity = m_velocityFunction(t);
		m_cachedAngularVelocity = m_angularVelocityFunction(t);
		p.rotation += m_cachedAngularVelocity;
		m_cachedColorRGB = m_colorFunction(t);
		m_cachedColor.r = static_cast<sf::Uint8>(m_cachedColorRGB.r);
		m_cachedColor.g = static_cast<sf::Uint8>(m_cachedColorRGB.g);
		m_cachedColor.b = static_cast<sf::Uint8>(m_cachedColorRGB.b);
		m_cachedColor.a = static_cast<sf::Uint8>(m_transparencyFunction(t));
		m_cachedSize = m_sizeFunction(t);
	}

	sf::Vector2f velocity = m_cachedVelocity;
	Math::rotate(velocity, p.angle);

	velocity *= m_speedModifier;
	velocity.y += PARTICLE_GRAVITY * m_gravityModifier;

	p.pos += velocity * deltaTime;

	if (m_following && m_target.valid() && m_target.has_component<TransformComponent>())
	{
		auto transform = m_target.component<TransformComponent>();
		float dx = transform->x - (p.pos.x + m_pos.x);
		float dy = transform->y - (p.pos.y + m_pos.y);

		if (dx*dx + dy*dy > m_goalRadius*m_goalRadius)
		{
			p.goalReached = false;
		}
		else if (dx*dx + dy*dy <= 25.f)
			p.goalReached = true;

		if (!p.goalReached)
		{
			velocity.x += dx*deltaTime*m_followSpeed;
			velocity.y += dy*deltaTime*m_followSpeed;

			p.pos -= m_pos - m_lastPos;
		}
	}
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

void ParticleEmitter::setTexture(Assets::Texture* texture)
{
	m_texture = texture;

	auto& rect = texture->getRect();
	for (uint32_t i = 0; i < m_vertices.size(); i += 4)
	{
		m_vertices[i + 0].texCoords = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top));
		m_vertices[i + 1].texCoords = sf::Vector2f(static_cast<float>(rect.left + rect.width), static_cast<float>(rect.top));
		m_vertices[i + 2].texCoords = sf::Vector2f(static_cast<float>(rect.left + rect.width), static_cast<float>(rect.top + rect.height));
		m_vertices[i + 3].texCoords = sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top + rect.height));
	}
}

void ParticleEmitter::spawnParticle()
{
	if (m_numActive < m_particles.size())
	{
		// Init
		Particle& particle = m_particles[m_numActive++];
		float angle = Random::getFloat(0.f, Math::PI*2.f);
		particle.rotation = 0.f;
		particle.lifetime = m_maxLifetime;

		float x = Random::getFloat(-m_spawnWidth*0.5f, m_spawnWidth*0.5f);
		float y = Random::getFloat(-m_spawnHeight*0.5f, m_spawnHeight*0.5f);

		particle.pos = sf::Vector2f(x, y);
		particle.angle = angle;
		particle.goalReached = false;
	}
}

/**
* @brief	Optimized quad update Considering:
* 			- The position of the particle and emitter
* 			- The rotation of the particle and emitter
* 			- Color and Size
*/
void ParticleEmitter::updateQuad(int vertexStart, Particle& p)
{
	float hw = m_cachedSize.x*0.5f;
	float hh = m_cachedSize.y*0.5f;

	float c = cosf(p.rotation);
	float s = sinf(p.rotation);

	float hwc = hw*c;
	float hws = hw*s;
	float hhc = hh*c;
	float hhs = hh*s;

	sf::Vertex& v0 = m_vertices[vertexStart];
	sf::Vertex& v1 = m_vertices[vertexStart + 1];
	sf::Vertex& v2 = m_vertices[vertexStart + 2];
	sf::Vertex& v3 = m_vertices[vertexStart + 3];

	float x = p.pos.x;
	float y = p.pos.y;

	c = cosf(m_rotation);
	s = sinf(m_rotation);

	v0.position = sf::Vector2f((-hwc + hhs + x)*c - (-hhc - hws + y)*s + m_pos.x, (-hhc - hws + y)*c + (-hwc + hhs + x)*s + m_pos.y);
	v1.position = sf::Vector2f((hwc + hhs + x) *c - (-hhc + hws + y)*s + m_pos.x, (-hhc + hws + y)*c + (hwc + hhs + x) *s + m_pos.y);
	v2.position = sf::Vector2f((hwc - hhs + x) *c - (hhc + hws + y) *s + m_pos.x, (hhc + hws + y) *c + (hwc - hhs + x) *s + m_pos.y);
	v3.position = sf::Vector2f((-hwc - hhs + x)*c - (hhc - hws + y) *s + m_pos.x, (hhc - hws + y) *c + (-hwc - hhs + x)*s + m_pos.y);

	v0.color = v1.color = v2.color = v3.color = m_cachedColor;
}

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.shader = nullptr;
	states.blendMode = m_blendMode;

	states.texture = m_texture->get();

	if (m_numActive > 0)
		target.draw(&m_vertices[0], m_numActive * 4, sf::Quads, states);
}