#include "ParticleSpawnSystem.h"
#include "../Events/DeathEvent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ParticleComponent.h"
#include "../Systems/ParticleSystem.h"
#include "../Utils/Functions.h"
#include "../GameConstants.h"
#include "../Components/ExplosionComponent.h"
#include "../GameGlobals.h"
#include "../EntityFactory.h"
#include "../Components/CellComponent.h"
#include "../Events/ExplosionCreatedEvent.h"
#include "../Components/EffectComponent.h"
#include "../Components/LayerComponent.h"
#include "../Events/ItemPickedUpEvent.h"

ParticleSpawnSystem::ParticleSpawnSystem(ParticleSystem* particleSystem, LayerManager* layerManager)
	:m_particleSystem(particleSystem), m_layerManager(layerManager)
{
}

void ParticleSpawnSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
}

void ParticleSpawnSystem::configure(entityx::EventManager& events)
{
	events.subscribe<DeathEvent>(*this);
	events.subscribe<ExplosionCreatedEvent>(*this);
	events.subscribe<ItemPickedUpEvent>(*this);
}

void ParticleSpawnSystem::receive(const DeathEvent& deathEvent)
{
	if (!deathEvent.dieingEntity.valid())
		return;

	auto entity = deathEvent.dieingEntity;

	if (entity.has_component<SpriteComponent>())
		entity.remove<SpriteComponent>();
	
	if (!entity.has_component<ParticleComponent>())
	{
		auto manager = m_particleSystem->getManager("block");
		auto emitter = manager->spawnEmitter();

		if (emitter)
		{
			entity.assign<ParticleComponent>();
			entity.component<ParticleComponent>()->emitter = emitter;

			emitter->spawnTime(1.f)
				.maxLifetime(1.f)
				.speedModifier(10.f)
				.burstParticleNumber(150)
				.velocityFunction(Gradient<sf::Vector2f>(GradientType::SMOOTH, sf::Vector2f(1, 1), sf::Vector2f(5, 5)))
				.burstTime(0.0f)
				.burstNumber(1)
				.spawnWidth(15)
				.spawnHeight(15)
				.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
				.sizeFunction(Gradient3<sf::Vector2f>(GradientType::SMOOTH, sf::Vector2f(60, 60), sf::Vector2f(5, 5), sf::Vector2f(35, 15)));
		}
	}
}

void ParticleSpawnSystem::receive(const ExplosionCreatedEvent& e)
{
	auto entity = e.entity;
	if (entity.has_component<ExplosionComponent>() && entity.has_component<CellComponent>() && entity.has_component<LayerComponent>())
	{
		auto layerComponent = entity.component<LayerComponent>();
		auto cellComponent = entity.component<CellComponent>();
		if (!m_layerManager->hasEntityWithComponent<EffectComponent>(layerComponent->layer, cellComponent->x, cellComponent->y))
			GameGlobals::entityFactory->createSmoke(cellComponent->x, cellComponent->y);
	}
}

void ParticleSpawnSystem::receive(const ItemPickedUpEvent& e)
{
	auto entity = e.itemReceiver;
	auto cell = entity.component<CellComponent>();
	assert(cell);
	GameGlobals::entityFactory->createBoostEffect(cell->x, cell->y, entity);
}