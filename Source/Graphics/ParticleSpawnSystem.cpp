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
#include "../Events/ItemCreatedEvent.h"
#include "ParticleEffects.h"

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
	events.subscribe<ItemCreatedEvent>(*this);
}

void ParticleSpawnSystem::receive(const DeathEvent& deathEvent)
{
	if (!deathEvent.dyingEntity.valid())
		return;

	auto entity = deathEvent.dyingEntity;

	if (entity.has_component<SpriteComponent>())
		entity.remove<SpriteComponent>();
	
	if (!entity.has_component<ParticleComponent>())
	{
		auto emitter = ParticleEffects::blockDeath();
		if (emitter)
			entity.assign<ParticleComponent>(emitter);
	}
}

void ParticleSpawnSystem::receive(const ExplosionCreatedEvent& e)
{
	auto entity = e.entity;

	if (!entity.valid())
		return;

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

	if (!entity.valid())
		return;

	auto cell = entity.component<CellComponent>();
	assert(cell);
	GameGlobals::entityFactory->createBoostEffect(cell->x, cell->y, entity);
}

void ParticleSpawnSystem::receive(const ItemCreatedEvent& e)
{
	if (e.entity)
	{
		auto item = e.entity;
		GameGlobals::entityFactory->createItemSpawnEffect(e.x, e.y, item);
	}
}