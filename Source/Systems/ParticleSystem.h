#pragma once
#include <entityx/System.h>
#include "../Graphics/EmitterManager.h"

struct DeathEvent;

class ParticleSystem : public entityx::System<ParticleSystem>, public entityx::Receiver<ParticleSystem>
{
public:
	ParticleSystem();
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	EmitterManager* getManager(const std::string& textureName);

	void configure(entityx::EventManager& events) override;

	void receive(const entityx::EntityDestroyedEvent& e);

private:
	void createManager(const std::string& textureName, uint32_t maxParticles, uint16_t maxEmitters);

private:
	std::unordered_map<std::string, EmitterManager> m_emitterManagers;
};

