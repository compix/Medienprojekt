#include "DeathSystem.h"
#include "../Events/DeathEvent.h"
#include "../Events/GameOverEvent.h"
#include "../Events/StartGameEvent.h"
#include "../Components/DestructionDelayComponent.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BodyComponent.h"
#include "../GameGlobals.h"
#include "../Components/PlayerComponent.h"
#include "../Components/HealthComponent.h"

DeathSystem::~DeathSystem()
{
	GameGlobals::events->unsubscribe<StartGameEvent>(*this);
	GameGlobals::events->unsubscribe<DeathEvent>(*this);
}

void DeathSystem::configure(entityx::EventManager& events)
{
	events.subscribe<StartGameEvent>(*this);
	events.subscribe<DeathEvent>(*this);
}

void DeathSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
}

void DeathSystem::receive(const DeathEvent& deathEvent)
{
	auto dyingEntity = deathEvent.dyingEntity;

	if (!dyingEntity.valid())
		return;

	auto delayComponent = dyingEntity.component<DestructionDelayComponent>();

	if (delayComponent)
	{
		if (!dyingEntity.has_component<DestructionComponent>())
			dyingEntity.assign<DestructionComponent>(delayComponent->seconds);
	}
	else
	{
		if (!dyingEntity.has_component<DestructionComponent>())
			dyingEntity.assign<DestructionComponent>();
	}

	if (!m_gameOver && dyingEntity.has_component<PlayerComponent>())
	{
		entityx::Entity lastPlayer;
		int alivePlayers = 0;
		for (auto player : GameGlobals::entities->entities_with_components<PlayerComponent, HealthComponent>())
		{
			auto health = player.component<HealthComponent>();
			if (health->value > 0)
			{
				alivePlayers++;
				lastPlayer = player;
			}
		}
		if (alivePlayers <= 1)
		{
			GameGlobals::events->emit<GameOverEvent>(lastPlayer);
			m_gameOver = true;
		}
	}
}

void DeathSystem::receive(const StartGameEvent& startGameEvent)
{
	m_gameOver = false;
}