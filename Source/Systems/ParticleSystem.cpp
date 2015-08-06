#include "ParticleSystem.h"
#include "../Components/ParticleComponent.h"
#include "../Components/TransformComponent.h"
#include "../Utils/AssetManagement/AssetManager.h"
#include "../GameGlobals.h"
#include "../Components/EffectComponent.h"
#include <ecstasy/core/Entity.h>

ParticleSystem::ParticleSystem()
:IteratingSystem(Family::all<ParticleComponent, TransformComponent>().get())
{
	createManager("light", 500, 500);
	createManager("block", 500, 500);
	createManager("smoke", 500, 500);
}

void ParticleSystem::addedToEngine(Engine *engine)
{
	IteratingSystem::addedToEngine(engine);
	m_connections += engine->entityRemoved.connect(this, &ParticleSystem::onEntityRemoved);
}

void ParticleSystem::removedFromEngine(Engine *engine)
{
	m_connections.removeAll();
}

void ParticleSystem::onEntityRemoved(Entity *entity)
{
	auto particleComponent = entity->get<ParticleComponent>();
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

void ParticleSystem::processEntity(Entity *e, float deltaTime)
{
	auto particleComponent = e->get<ParticleComponent>();
	auto transform = e->get<TransformComponent>();

	if (particleComponent->emitter->isFollowing())
	{
		auto target = getEngine()->getEntity(particleComponent->emitter->m_targetId);

		if (target && !target->isScheduledForRemoval() && target->has<TransformComponent>() && e->has<EffectComponent>())
		{
			auto targetTransform = target->get<TransformComponent>();
			transform->x = targetTransform->x;
			transform->y = targetTransform->y + 1.f; // + 1.f = little hack to force drawing the particle effect on top of the target
		}
	}
	particleComponent->emitter->rotation(transform->rotation);
	particleComponent->emitter->position(transform->x, transform->y);
}

void ParticleSystem::update(float dt)
{
	IteratingSystem::update(dt);

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