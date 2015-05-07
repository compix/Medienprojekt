#include "Game.h"
#include "Systems/AnimationSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/BodySystem.h"
#include "Systems/RenderSystem.h"
#include <Box2D/Box2D.h>
#include "Utils/LevelGenerator.h"
#include "Systems/DeathSystem.h"
#include "Systems/HealthSystem.h"
#include "Utils/make_unique.h"
#include "Systems/ExplosionSystem.h"
#include "Systems/DestructionSystem.h"
#include "Systems/DamageSystem.h"
#include "Utils/Random.h"

Game::Game(sf::RenderWindow* window, InputManager &inputManager)
	:m_timer(1.f)
{

	b2Vec2 gravity(0.0f, 10.0f);
	m_world = std::make_unique<b2World>(gravity);

	m_layerManager = std::make_unique<LayerManager>();
	m_layerManager->configure(events);

	m_textureLoader = std::make_unique<TextureLoader>();
	m_textureLoader->loadAllFromJson("assets/json/textures.json");
	m_entityFactory = std::make_unique<EntityFactory>(this, m_textureLoader.get(), m_world.get(), m_layerManager.get());

	systems.add<DamageSystem>(m_layerManager.get());
	systems.add<DestructionSystem>();
	systems.add<ExplosionSystem>(m_entityFactory.get(), m_layerManager.get());
	systems.add<HealthSystem>();
	systems.add<DeathSystem>();
	systems.add<BodySystem>();
	systems.add<InputSystem>(inputManager);
	systems.add<AnimationSystem>();
	systems.add<RenderSystem>(window, m_layerManager.get());
	systems.configure();

	EntityLayer* entityLayer = m_layerManager->createLayer(21, 21, 0);

	LevelGenerator levelGenerator(m_entityFactory.get(), 21, 21);
	levelGenerator.generateRandomLevel();
}

Game::~Game() { }

void Game::update(TimeDelta dt)
{
	m_world->Step((float32)dt, 6, 2);
	systems.update_all(dt);
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
		} while (m_layerManager->hasSolidBlock(0, cellX, cellY));

		m_entityFactory->createExplosion(cellY, cellX, 3, 0.06f);
	}
}