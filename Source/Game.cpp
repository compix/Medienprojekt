﻿#include "Game.h"
#include "Systems/AnimationSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/BodySystem.h"
#include "Systems/RenderSystem.h"
#include "Utils/LevelGenerator.h"
#include "Systems/DeathSystem.h"
#include "Systems/HealthSystem.h"
#include "Utils/make_unique.h"
#include "BodyFactory.h"
#include "Systems/ExplosionSystem.h"
#include "Systems/DestructionSystem.h"
#include "Systems/DamageSystem.h"
#include "Utils/Random.h"
#include "GameConstants.h"
#include "Systems/TimerSystem.h"
#include "Systems/BombSystem.h"


Game::Game(sf::RenderWindow* window, InputManager &inputManager, EventManager &events, SFMLDebugDraw* debugDraw)
	:m_timer(1.f), m_events(events), m_entities(events), m_systems(m_entities, events)
{

	/*Setup PhysixSystem*/
	m_PhysixSystem = new PhysixSystem(6, 3, GameConstants::S_SCALE);
	m_PhysixSystem->SetDebugDrawer(debugDraw);
	BodyFactory::m_World = m_PhysixSystem->GetWorld();
	/*Setup PhysixSystem End*/


	m_layerManager = std::make_unique<LayerManager>();
	m_layerManager->createLayer(21, 21, 0);
	m_layerManager->configure(events);

	m_textureLoader = std::make_unique<TextureLoader>();
	m_textureLoader->loadAllFromJson("assets/json/textures.json");
	m_entityFactory = std::make_unique<EntityFactory>(m_entities, m_textureLoader.get(), m_PhysixSystem, m_layerManager.get());

	m_systems.add<TimerSystem>();
	m_systems.add<BombSystem>(m_entityFactory.get());
	m_systems.add<DamageSystem>(m_layerManager.get());
	m_systems.add<DestructionSystem>();
	m_systems.add<ExplosionSystem>(m_entityFactory.get(), m_layerManager.get());
	m_systems.add<HealthSystem>();
	m_systems.add<DeathSystem>();
	m_systems.add<BodySystem>();
	m_systems.add<InputSystem>(inputManager);
	m_systems.add<AnimationSystem>();
	m_systems.add<RenderSystem>(window, m_layerManager.get());
	m_systems.configure();

	LevelGenerator levelGenerator(m_entityFactory.get(), 21, 21);
	levelGenerator.generateRandomLevel();
}

Game::~Game() { 
	delete m_PhysixSystem;
}

void Game::update(TimeDelta dt)
{
	m_PhysixSystem->Update(dt);
	m_systems.update_all(dt);
	m_PhysixSystem->DrawDebug();
	m_layerManager->update();

	testExplosions(dt);
}

void Game::testExplosions(TimeDelta dt)
{
	m_timer -= (float)dt;

	if (m_timer <= 0.f)
	{
		m_timer = 1.f;

		int cellX, cellY;

		do
		{
			cellX = Random::getInt(1, 19);
			cellY = Random::getInt(1, 19);
		} while (!m_layerManager->isFree(0, cellX, cellY));

		m_entityFactory->createBomb(cellY, cellX);
	}
}