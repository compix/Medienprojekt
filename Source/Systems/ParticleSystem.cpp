#include "ParticleSystem.h"
#include "../Components/ParticleComponent.h"
#include "../Components/TransformComponent.h"
#include "../Utils/AssetManagement/AssetManager.h"
#include "../GameGlobals.h"
#include "../Components/EffectComponent.h"

ParticleSystem::ParticleSystem()
{
	createManager("light", 500, 500);
	createManager("block", 500, 500);
	createManager("smoke", 500, 500);
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

void ParticleSystem::createManager(const std::string& textureName, uint32_t maxParticles, uint16_t maxEmitters)
{
	m_particleManagers.insert({ textureName, ParticleManager(maxParticles, GameGlobals::assetManager->getTexture(textureName), maxEmitters) });
}

void ParticleSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	for (auto e : entityManager.entities_with_components<ParticleComponent, TransformComponent>())
	{
		auto particleComponent = e.component<ParticleComponent>();
		auto transform = e.component<TransformComponent>();

		if (particleComponent->emitter->isFollowing())
		{
			auto target = particleComponent->emitter->m_target;
			auto targetTransform = target.component<TransformComponent>();
			if (targetTransform && e.has_component<EffectComponent>())
			{
				transform->x = targetTransform->x;
				transform->y = targetTransform->y + 1.f; // + 1.f = little hack to force drawing the particle effect on top of the target
			}
		}

		particleComponent->emitter->rotation(transform->rotation);
		particleComponent->emitter->position(transform->x, transform->y);
	}

	for (auto& m : m_particleManagers)
	{
		m.second.update((float)dt);
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