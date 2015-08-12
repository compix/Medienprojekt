#pragma once
#include <entityx/System.h>

struct StartGameEvent;
struct DeathEvent;

class DeathSystem : public entityx::System<DeathSystem>, public entityx::Receiver<DeathSystem>
{
public:
	~DeathSystem();
	void configure(entityx::EventManager& events) override;
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	void receive(const DeathEvent& deathEvent);
	void receive(const StartGameEvent& startGameEvent);

private:
	bool m_gameOver = false;
};
