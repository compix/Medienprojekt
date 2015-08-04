#pragma once
#include "../Graphics/ParticleManager.h"
#include <ecstasy/core/Engine.h>
#include <ecstasy/systems/IteratingSystem.h>
#include <signal11/Signal.h>

class ParticleSystem : public IteratingSystem<ParticleSystem>
{
public:
	ParticleSystem();
	void update(float dt) override;
	void processEntity(Entity *entity, float deltaTime) override;

	ParticleManager* getManager(const std::string& textureName);

	void addedToEngine(Engine *engine) override;
	void removedFromEngine(Engine *engine) override;

private:
	void onEntityRemoved(Entity *entity);
	void createManager(const std::string& textureName, uint32_t maxParticles, uint16_t maxEmitters);

private:
	std::unordered_map<std::string, ParticleManager> m_particleManagers;
	ConnectionScope m_connections;
};

