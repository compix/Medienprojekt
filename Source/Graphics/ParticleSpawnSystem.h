#pragma once
#include <ecstasy/core/EntitySystem.h>
#include <ecstasy/core/Engine.h>


enum class Direction : uint8_t;
class LayerManager;
class ParticleSystem;

class ParticleSpawnSystem : public EntitySystem<ParticleSpawnSystem>
{
public:
	ParticleSpawnSystem(ParticleSystem* particleSystem, LayerManager* layerManager);

	void update(float dt) override;
	void addedToEngine(Engine *engine) override;
	
private:
	void onDeath(Entity *dyingEntity);
	void onExplosionCreated(Entity *entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime);
	void onItemPickedUp(Entity *item, Entity *itemReceiver);
	
private:
	ParticleSystem* m_particleSystem;
	LayerManager* m_layerManager;
	Engine *m_engine = nullptr;
};