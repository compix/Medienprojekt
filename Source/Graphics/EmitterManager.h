#pragma once
#include <SFML/Graphics/Vertex.hpp>
#include <vector>
#include "Particle.h"
#include <memory>
#include "../Utils/AssetManagement/TexturePacker.h"

class ParticleEmitter;

const float PARTICLE_UPDATE_FREQUENCY = 1.f/60.f;

class EmitterManager 
{
	typedef std::vector<std::shared_ptr<ParticleEmitter>> EmitterContainer;
	friend class ParticleEmitter;
public:
	EmitterManager();
	EmitterManager(uint32_t defaultParticlesPerEmitter, uint16_t maxEmitters);
	EmitterManager(uint32_t defaultParticlesPerEmitter, Assets::Texture* texture, uint16_t maxEmitters);
	void update(float deltaTime);

	void setTexture(uint32_t maxParticles);
	ParticleEmitter* spawnEmitter();
	ParticleEmitter* spawnEmitter(uint32_t maxParticles);
	void createEmitters(uint32_t maxParticlesPerEmitter, uint16_t maxEmitters);

private:
	Assets::Texture* m_texture;

	uint32_t m_defaultParticlesPerEmitter;

	std::unordered_map<uint32_t, uint16_t> m_activeMap;
	std::unordered_map<uint32_t, EmitterContainer> m_emitterContainerMap;


	float m_timeTillUpdate;
};