#pragma once
#include <entityx/Event.h>
#include <entityx/System.h>
#include "../LayerManager.h"

struct SkillEvent;
class LayerManager;
class b2Body;
enum class Direction : uint8_t;

class BlinkSystem : public entityx::System<BlinkSystem>, public entityx::Receiver<BlinkSystem>
{
public:
	BlinkSystem(LayerManager* layerManager);
	~BlinkSystem();
	void configure(entityx::EventManager &event_manager) override;
	void update(EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
	void receive(const SkillEvent& evt);
private:
	LayerManager* m_layerManager;

	void setVelocity(b2Body* body, Direction direction);
	void spawnAfterImages(entityx::Entity &entity, entityx::TimeDelta dt);
};