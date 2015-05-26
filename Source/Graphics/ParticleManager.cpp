#include "ParticleManager.h"
#include "../Utils/Math.h"
#include "ParticleEmitter.h"
#include "../Utils/Colors.h"
#include <iostream>

ParticleManager::ParticleManager() : ParticleManager(50000)
{

}

ParticleManager::ParticleManager(uint32_t maxParticles)
	: m_maxParticles(maxParticles), m_numActive(0), m_timeTillUpdate(PARTICLE_UPDATE_FREQUENCY)
{
	m_particles.resize(m_maxParticles);
	m_vertices.resize(m_maxParticles * 4);
}

ParticleManager::ParticleManager(uint32_t maxParticles, Assets::Texture* texture)
	: ParticleManager(maxParticles)
{
	setTexture(texture);
}

void ParticleManager::update(sf::RenderTarget& target, float deltaTime)
{
	m_timeTillUpdate -= deltaTime;

	if (m_timeTillUpdate <= 0.f)
	{
		float updateTime = deltaTime < PARTICLE_UPDATE_FREQUENCY ? PARTICLE_UPDATE_FREQUENCY : deltaTime;

		// Update emitters
		for (auto e : m_emitters)
			if (!e->m_scheduledForRemoval)
				e->update(updateTime);

		sf::Color color;
		sf::Vector2f size;

		// Update particles
		int i = 0;
		while (i < m_numActive)
		{
			Particle& p = m_particles[i];

			p.lifetime -= updateTime;
			if (p.lifetime <= 0.f || p.emitter->m_scheduledForRemoval)
			{
				m_numActive--;

				m_particles[i] = m_particles[m_numActive]; // Place the last active particle at the position of the dead particle
				continue;
			}


			p.emitter->update(p, updateTime, color, size);
			updateQuad(i * 4, p, color, size);

			i++;
		}

		// Remove emitters if necessary
		auto it = m_emitters.begin();
		while (it != m_emitters.end())
			if ((*it)->m_scheduledForRemoval)
				it = m_emitters.erase(it);
			else
				++it;

		m_timeTillUpdate += PARTICLE_UPDATE_FREQUENCY;
	}

	target.draw(*this);
}

void ParticleManager::setTexture(Assets::Texture* texture)
{
	m_texture = texture;

	//float texWidth = (float)texture.getSize().x;
	//float texHeight = (float)texture.getSize().y;

	auto& rect = texture->getRect();
	for (int i = 0; i < m_vertices.size(); i+=4)
	{
		m_vertices[i + 0].texCoords = sf::Vector2f(rect.left, rect.top);
		m_vertices[i + 1].texCoords = sf::Vector2f(rect.left + rect.width, rect.top);
		m_vertices[i + 2].texCoords = sf::Vector2f(rect.left + rect.width, rect.top + rect.height);
		m_vertices[i + 3].texCoords = sf::Vector2f(rect.left, rect.top + rect.height);
	}
}

ParticleEmitter* ParticleManager::spawnEmitter()
{
	m_emitters.push_back(std::make_shared<ParticleEmitter>());

	auto e = m_emitters[m_emitters.size() - 1];
	e->setParticleManager(this);

	return e.get();
}

void ParticleManager::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.shader = nullptr;
	states.blendMode = sf::BlendAdd;
	
	states.texture = m_texture->get();
	
	if (m_numActive > 0)
		target.draw(&m_vertices[0], m_numActive * 4, sf::Quads, states);
}

Particle* ParticleManager::spawnParticle()
{
	if (hasSpace())
		return &m_particles[m_numActive++];

	return nullptr;
}

/**
 * @brief	Optimized quad update Considering: 
 * 			- The position of the particle and emitter
 * 			- The rotation of the particle and emitter
 * 			- Color and Size
 */
void ParticleManager::updateQuad(int vertexStart, Particle& p, const sf::Color& color, const sf::Vector2f& size)
{
	float hw = size.x*0.5f;
	float hh = size.y*0.5f;

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

	c = cosf(p.emitter->m_rotation);
	s = sinf(p.emitter->m_rotation);

	v0.position = sf::Vector2f((-hwc + hhs + x)*c - (-hhc - hws + y)*s + p.emitter->m_pos.x, (-hhc - hws + y)*c + (-hwc + hhs + x)*s + p.emitter->m_pos.y);
	v1.position = sf::Vector2f((hwc + hhs + x) *c - (-hhc + hws + y)*s + p.emitter->m_pos.x, (-hhc + hws + y)*c + (hwc + hhs + x) *s + p.emitter->m_pos.y);
	v2.position = sf::Vector2f((hwc - hhs + x) *c - (hhc + hws + y) *s + p.emitter->m_pos.x, (hhc + hws + y) *c + (hwc - hhs + x) *s + p.emitter->m_pos.y);
	v3.position = sf::Vector2f((-hwc - hhs + x)*c - (hhc - hws + y) *s + p.emitter->m_pos.x, (hhc - hws + y) *c + (-hwc - hhs + x)*s + p.emitter->m_pos.y);

	v0.color = v1.color = v2.color = v3.color = color;
}