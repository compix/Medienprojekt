#include "Game.h"
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
#include "Lighting/Light.h"
#include "Utils/ShaderManager.h"
#include "Systems/LightSystem.h"
#include "Systems/InputHandleSystem.h"
#include "Systems/InventorySystem.h"
#include "GameGlobals.h"
#include "Utils/Functions.h"
#include "Utils/Math.h"
#include "Systems/ParticleSystem.h"
#include "Graphics/ParticleEmitter.h"


Game::Game()
	:m_timer(1.f), m_entities(*GameGlobals::events), m_systems(m_entities, *GameGlobals::events), m_debugDraw(*GameGlobals::window), m_PhysixSystem(nullptr)
{
	GameGlobals::entities = &m_entities;
}

Game::~Game() { 
	if (m_PhysixSystem)
		delete m_PhysixSystem;
}

void Game::init(uint8_t width, uint8_t height)
{
	m_width = width;
	m_height = height;

	m_shaderManager.updateScreenResolution(GameGlobals::window->getSize());

	m_debugDraw.SetFlags(b2Draw::e_shapeBit);
	//m_debugDraw.AppendFlags(b2Draw::e_jointBit);
	//m_debugDraw.AppendFlags(b2Draw::e_centerOfMassBit);
	//m_debugDraw.AppendFlags(b2Draw::e_aabbBit);

	/*Setup PhysixSystem*/
	m_PhysixSystem = new PhysixSystem(6, 3, GameConstants::S_SCALE);
	m_PhysixSystem->SetDebugDrawer(&m_debugDraw);
	BodyFactory::m_World = m_PhysixSystem->GetWorld();
	/*Setup PhysixSystem End*/


	m_layerManager = std::make_unique<LayerManager>();
	m_layerManager->createLayer(width, height, 0);
	m_layerManager->createLayer(width, height, -1);
	m_layerManager->configure(*GameGlobals::events);

	m_entityFactory = std::make_unique<EntityFactory>(m_PhysixSystem, m_layerManager.get(), &m_shaderManager, &m_systems);
	GameGlobals::entityFactory = m_entityFactory.get();

	addSystems();
	m_systems.configure();

	m_light.create(sf::Vector2f(35.f, 60.f), sf::Color::Yellow, 200.f, 360.f, 0.f);

	auto particleSystem = m_systems.system<ParticleSystem>();
	m_particleEmitter = particleSystem->getManager("light")->spawnEmitter();

	// This Particle Emitter is just for tests.
	m_particleEmitter->spawnTime(0.003f)
		.position(GameGlobals::window->getSize().x*0.5f, GameGlobals::window->getSize().y*0.5f)
		.maxLifetime(5.f)
		.gravityModifier(5.f)
		.velocityFunction([](float t) { t = t * 2 - 1; return sf::Vector2f(t, sinf(t)*100.f); })
		.angularVelocityFunction([](float t) { t = t * 2 - 1; return t*t*0.1f; })
		.sizeFunction([](float t) { t = t * 2 - 1; return sf::Vector2f(15 - t*t*50.f, 15 - t*t*t*20.f); })
		.transparencyFunction([](float t) { return t < 0.1 ? 15.f : 255 - t * 255; })
		.colorFunction([](float t) { return RGB(0.f, Math::smootherstep(234, 23, t), Math::regress(255, 66, t)); });

	initialized = true;
}

void Game::update(TimeDelta dt)
{
	if (!initialized)
		return;

	m_PhysixSystem->Update(dt);
	m_systems.update_all(dt);
	m_PhysixSystem->DrawDebug();
	m_layerManager->update();

	m_light.create(sf::Vector2f(m_mousePos.x, m_mousePos.y), sf::Color::Yellow, 200.f, 360.f, 0.f);
	m_light.setShader(m_shaderManager.getLightShader());

	GameGlobals::window->draw(m_light);
}

void LocalGame::addSystems()
{
	m_systems.add<InventorySystem>();
	m_systems.add<TimerSystem>();
	m_systems.add<BombSystem>();
	m_systems.add<DamageSystem>(m_layerManager.get());
	m_systems.add<DestructionSystem>();
	m_systems.add<ExplosionSystem>(m_layerManager.get());
	m_systems.add<HealthSystem>();
	m_systems.add<DeathSystem>();
	m_systems.add<BodySystem>();
	m_systems.add<InputSystem>();
	m_systems.add<InputHandleSystem>();
	m_systems.add<AnimationSystem>();
	m_systems.add<RenderSystem>(m_layerManager.get());
	m_systems.add<ParticleSystem>();
	m_systems.add<LightSystem>();
}

void LocalGame::init(uint8_t width, uint8_t height)
{
	Game::init(width, height);
	LevelGenerator levelGenerator(height, width);
	levelGenerator.generateRandomLevel();
}

void ClientGame::addSystems()
{
	m_systems.add<InputSystem>();
//	m_systems.add<ClientInputHandleSystem>(); // fixme
	m_systems.add<AnimationSystem>();
	m_systems.add<RenderSystem>(m_layerManager.get());
	m_systems.add<ParticleSystem>();
	m_systems.add<LightSystem>();
}