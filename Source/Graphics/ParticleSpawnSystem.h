#pragma once
#include <entityx/entityx.h>

struct ItemCreatedEvent;
struct ItemPickedUpEvent;
class LayerManager;
struct ExplosionCreatedEvent;
class ParticleSystem;
struct DeathEvent;

class ParticleSpawnSystem : public entityx::System<ParticleSpawnSystem>, public entityx::Receiver<ParticleSpawnSystem>
{
public:
	ParticleSpawnSystem(ParticleSystem* particleSystem, LayerManager* layerManager);

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
	void configure(entityx::EventManager& events) override;
	void receive(const DeathEvent& deathEvent);
	void receive(const ExplosionCreatedEvent& e);
	void receive(const ItemPickedUpEvent& e);
	void receive(const ItemCreatedEvent& e);
private:
	ParticleSystem* m_particleSystem;
	LayerManager* m_layerManager;
};