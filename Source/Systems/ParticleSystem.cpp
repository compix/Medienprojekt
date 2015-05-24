#include "ParticleSystem.h"
#include "../Components/ParticleComponent.h"
#include "../Components/TransformComponent.h"
#include "../Utils/TextureLoader.h"
#include "../GameGlobals.h"

ParticleSystem::ParticleSystem()
{
	createManager("light");
}

void ParticleSystem::configure(entityx::EventManager& events)
{
	events.subscribe<entityx::EntityDestroyedEvent>(*this);
}

void ParticleSystem::receive(const entityx::EntityDestroyedEvent& e)
{
	auto entity = e.entity;
	auto particleComponent = entity.component<ParticleComponent>();
	if (particleComponent)
	{
		particleComponent->emitter->remove();
		particleComponent->emitter = nullptr;
	}
}

void ParticleSystem::createManager(const std::string& textureName)
{
	m_particleManagers.insert({ textureName, ParticleManager(50000, GameGlobals::textures->get(textureName)) });
}

void ParticleSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto e : entityManager.entities_with_components<ParticleComponent, TransformComponent>())
	{
		auto particleComponent = e.component<ParticleComponent>();
		auto transform = e.component<TransformComponent>();

		particleComponent->emitter->position(transform->x, transform->y);
		particleComponent->emitter->rotation(transform->rotation);
	}

	for (auto& m : m_particleManagers)
	{
		m.second.update(*GameGlobals::window, (float)dt);
	}
}

/**
 * @brief	Make sure the manager for the given texture exists. Create it in the ParticleSystem if not.
 */
ParticleManager* ParticleSystem::getManager(const std::string& textureName)
{
	assert(m_particleManagers.count(textureName) != 0);

	return &m_particleManagers[textureName];
}