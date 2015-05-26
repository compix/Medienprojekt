#pragma once
#include <SFML/Graphics/Vertex.hpp>
#include <vector>
#include "Particle.h"
#include <memory>
#include "../Utils/AssetManagement/TexturePacker.h"

class ParticleEmitter;

const float PARTICLE_UPDATE_FREQUENCY = 1.f/60.f;

class ParticleManager : public sf::Drawable
{
	friend class ParticleEmitter;
public:
	ParticleManager();
	ParticleManager(uint32_t maxParticles);
	ParticleManager(uint32_t maxParticles, Assets::Texture* texture);
	void update(sf::RenderTarget& target, float deltaTime);

	bool hasSpace() { return m_numActive < m_maxParticles; }

	void setTexture(Assets::Texture* texture);
	ParticleEmitter* spawnEmitter();

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	Particle* spawnParticle();
	void updateQuad(int vertexStart, Particle& p, const sf::Color& color, const sf::Vector2f& size);

private:
	Assets::Texture* m_texture;

	uint32_t m_maxParticles;
	uint32_t m_numActive;
	std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
	std::vector<Particle> m_particles;
	std::vector<sf::Vertex> m_vertices;

	float m_timeTillUpdate;
};