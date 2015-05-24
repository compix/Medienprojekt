#pragma once
#include <entityx/System.h>

struct BombExplodedEvent;

class InventorySystem : public entityx::System<InventorySystem>, public entityx::Receiver<InventorySystem>
{
public:
	~InventorySystem();

	void configure(entityx::EventManager& events) override;

	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const BombExplodedEvent& e);
};

