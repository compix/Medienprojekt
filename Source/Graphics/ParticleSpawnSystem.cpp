#include "ParticleSpawnSystem.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ParticleComponent.h"
#include "../Systems/ParticleSystem.h"
#include "../Utils/Functions.h"
#include "../GameConstants.h"
#include "../Components/ExplosionComponent.h"
#include "../GameGlobals.h"
#include "../EntityFactory.h"
#include "../Components/CellComponent.h"
#include "../Components/EffectComponent.h"
#include "../Components/LayerComponent.h"

ParticleSpawnSystem::ParticleSpawnSystem(ParticleSystem* particleSystem, LayerManager* layerManager)
	:m_particleSystem(particleSystem), m_layerManager(layerManager)
{
}

void ParticleSpawnSystem::update(float dt)
{
}

void ParticleSpawnSystem::addedToEngine(Engine *engine)
{
	m_engine = engine;
	GameGlobals::events->death.connect(this, ParticleSpawnSystem::onDeath);
	GameGlobals::events->explosionCreated.connect(this, ParticleSpawnSystem::onExplosionCreated);
	GameGlobals::events->itemPickedUp.connect(this, ParticleSpawnSystem::onItemPickedUp);
}

void ParticleSpawnSystem::onDeath(Entity *dyingEntity)
{
	if (!dyingEntity->isValid())
		return;

	auto entity = dyingEntity;

	if (entity->has<SpriteComponent>())
		entity->remove<SpriteComponent>();
	
	if (!entity->has<ParticleComponent>())
	{
		auto manager = m_particleSystem->getManager("block");
		auto emitter = manager->spawnEmitter();

		if (emitter)
		{
			entity->add(m_engine->createComponent<ParticleComponent>());
			entity->get<ParticleComponent>()->emitter = emitter;

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

void ParticleSpawnSystem::onExplosionCreated(Entity *entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime)
{
	if (!entity->isValid())
		return;

	if (entity->has<ExplosionComponent>() && entity->has<CellComponent>() && entity->has<LayerComponent>())
	{
		auto layerComponent = entity->get<LayerComponent>();
		auto cellComponent = entity->get<CellComponent>();
		if (!m_layerManager->hasEntityWithComponent<EffectComponent>(layerComponent->layer, cellComponent->x, cellComponent->y))
			GameGlobals::entityFactory->createSmoke(cellComponent->x, cellComponent->y);
	}
}

void ParticleSpawnSystem::onItemPickedUp(Entity *item, Entity *itemReceiver)
{
	if (!itemReceiver->isValid())
		return;

	auto cell = itemReceiver->get<CellComponent>();
	assert(cell);
	GameGlobals::entityFactory->createBoostEffect(cell->x, cell->y, itemReceiver);
}