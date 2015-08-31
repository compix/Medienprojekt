#include "EmitterManager.h"
#include "../Utils/Math.h"
#include "ParticleEmitter.h"
#include "../Utils/Colors.h"
#include <iostream>

EmitterManager::EmitterManager() : EmitterManager(10000, 50)
{

}

EmitterManager::EmitterManager(uint32_t defaultParticlesPerEmitter, uint16_t maxEmitters)
	: m_defaultParticlesPerEmitter(defaultParticlesPerEmitter), m_timeTillUpdate(PARTICLE_UPDATE_FREQUENCY), m_texture(nullptr)
{
	createEmitters(defaultParticlesPerEmitter, maxEmitters);
}

EmitterManager::EmitterManager(uint32_t defaultParticlesPerEmitter, Assets::Texture* texture, uint16_t maxEmitters)
	: EmitterManager(defaultParticlesPerEmitter, maxEmitters)
{
	m_texture = texture;
	setTexture(defaultParticlesPerEmitter);
}

void EmitterManager::update(float deltaTime)
{
	m_timeTillUpdate -= deltaTime;

	if (m_timeTillUpdate <= 0.f)
	{
		float updateTime = deltaTime < PARTICLE_UPDATE_FREQUENCY ? PARTICLE_UPDATE_FREQUENCY : deltaTime;

		// Update emitters		
		for (auto& pair : m_emitterContainerMap)
		{
			auto& emitters = pair.second;
			auto& activeEmitters = m_activeMap[pair.first];
			uint16_t i = 0;
			while (i < activeEmitters)
			{
				auto e = emitters[i];
				if (!e->m_scheduledForRemoval)
				{
					e->update(updateTime);
					++i;
				}
				else
					std::swap(emitters[i], emitters[--activeEmitters]);
			}
		}



		m_timeTillUpdate += PARTICLE_UPDATE_FREQUENCY;
	}
}

void EmitterManager::setTexture(uint32_t maxParticles)
{
	auto& emitters = m_emitterContainerMap[maxParticles];

	for (auto& e : emitters)
		e->setTexture(m_texture);
}

ParticleEmitter* EmitterManager::spawnEmitter()
{
	return spawnEmitter(m_defaultParticlesPerEmitter);
}

/**
 * @brief	Don't use this dynamically if the given number of particles wasn't created at startup.
 * 			Use createEmitters() in the ParticleSystem with the max number of particles.
 */
ParticleEmitter* EmitterManager::spawnEmitter(uint32_t maxParticles)
{
	if (m_emitterContainerMap.count(maxParticles) == 0)
	{
		std::cout << "WARNING: spawnEmitter(" << maxParticles << ") was called without emitters being initialized for the given number of particles.\n"
			<< " Use createEmitters() in the ParticleSystem.\n" << "CREATING 10 EMITTERS FOR " << maxParticles << " PARTICLES...\n";

		createEmitters(maxParticles, 10);
	}

	auto& emitters = m_emitterContainerMap[maxParticles];
	auto& activeEmitters = m_activeMap[maxParticles];

	if (activeEmitters >= emitters.size())
		return nullptr;

	auto e = emitters[activeEmitters++];
	e->refresh();

	return e.get();
}

void EmitterManager::createEmitters(uint32_t maxParticlesPerEmitter, uint16_t maxEmitters)
{
	if (m_emitterContainerMap.count(maxParticlesPerEmitter) > 0)
	{
		std::cout << "WARNING: Attempted to create emitters for " << maxParticlesPerEmitter << " particles a second time." << std::endl;
		return;
	}

	m_emitterContainerMap[maxParticlesPerEmitter].resize(maxEmitters);
	m_activeMap[maxParticlesPerEmitter] = 0;

	for (uint16_t i = 0; i < maxEmitters; ++i)
		m_emitterContainerMap[maxParticlesPerEmitter][i] = std::make_shared<ParticleEmitter>(maxParticlesPerEmitter);

	if (m_texture)
		setTexture(maxParticlesPerEmitter);
}