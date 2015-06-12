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
#include "Animation/AnimatorManager.h"
#include "Graphics/ParticleSpawnSystem.h"
#include "Systems/ItemSystem.h"
#include <SFML/Audio/Music.hpp>
#include "Systems/SoundSystem.h"
#include "Utils/AssetManagement/AssetManager.h"
#include "Systems/MusicSystem.h"
#include "Events/StartGameEvent.h"

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

	refreshView();

	m_debugDraw.SetFlags(b2Draw::e_shapeBit);
	//m_debugDraw.AppendFlags(b2Draw::e_jointBit);
	//m_debugDraw.AppendFlags(b2Draw::e_centerOfMassBit);
	//m_debugDraw.AppendFlags(b2Draw::e_aabbBit);

	/*Setup PhysixSystem*/
	m_PhysixSystem = new PhysixSystem(6, 3, GameConstants::S_SCALE);
	m_PhysixSystem->setContactListener(&listener);
	m_PhysixSystem->SetDebugDrawer(&m_debugDraw);
	BodyFactory::m_World = m_PhysixSystem->GetWorld();
	/*Setup PhysixSystem End*/

	

	m_layerManager = std::make_unique<LayerManager>();
	m_layerManager->createLayer(width, height, GameConstants::MAIN_LAYER);
	m_layerManager->createLayer(width, height, GameConstants::FLOOR_LAYER);
	m_layerManager->configure(*GameGlobals::events);

	m_entityFactory = std::make_unique<EntityFactory>(m_PhysixSystem, m_layerManager.get(), &m_shaderManager, &m_systems);
	GameGlobals::entityFactory = m_entityFactory.get();

	addSystems();
	m_systems.configure();

	//m_light.create(sf::Vector2f(35.f, 60.f), sf::Color::Yellow, 200.f, 360.f, 0.f);

	auto particleSystem = m_systems.system<ParticleSystem>();
	m_particleEmitter = particleSystem->getManager("light")->spawnEmitter();

	// This Particle Emitter is just for tests.
	m_particleEmitter->
		position(GameGlobals::window->getSize().x*0.5f, GameGlobals::window->getSize().y*0.5f)
		.spawnTime(0.008f)
		.maxLifetime(0.5f)
		.speedModifier(10.f)
		.gravityModifier(-7.f)
		.velocityFunction([](float t) { t = t*2.f - 1.f; return sf::Vector2f(t*5.f, t*t*t*t*t*15.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(5, 5), sf::Vector2f(10, 10)))
		.spawnWidth(15)
		.spawnHeight(50)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(0, 255, 252), RGB(42, 255, 0)));

	initialized = true;

	GameGlobals::events->emit<StartGameEvent>();
	
}

void Game::update(TimeDelta dt)
{
	if (!initialized)
		return;

	m_PhysixSystem->Update(dt);
	m_systems.update_all(dt);
	//m_PhysixSystem->DrawDebug();
	m_layerManager->update();

	//m_light.create(sf::Vector2f(m_mousePos.x, m_mousePos.y), sf::Color::Yellow, 200.f, 360.f, 0.f);
	//m_light.setShader(m_shaderManager.getLightShader());

	GameGlobals::window->draw(m_light);
	GameGlobals::window->draw(*m_particleEmitter);
}

void Game::refreshView()
{
	float min = std::min(GameGlobals::window->getSize().x, GameGlobals::window->getSize().y);
	float levelLength = std::min(getWidth()*GameConstants::CELL_WIDTH, getHeight()*GameConstants::CELL_HEIGHT);
	float scaleFactor = std::max(1.f, levelLength / min);
	m_view.reset(sf::FloatRect(0, 0, GameGlobals::window->getSize().x * scaleFactor, GameGlobals::window->getSize().y * scaleFactor));
	m_view.setCenter(GameGlobals::game->getWidth()*GameConstants::CELL_WIDTH*0.5f, GameGlobals::game->getHeight()*GameConstants::CELL_HEIGHT*0.5f);
	m_shaderManager.updateScreenResolution(GameGlobals::window->getSize());
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
	m_systems.add<ItemSystem>(m_layerManager.get());
	m_systems.add<ParticleSpawnSystem>(m_systems.system<ParticleSystem>().get(), m_layerManager.get());
	m_systems.add<MusicSystem>();
	m_systems.add<SoundSystem>();
}

void LocalGame::init(uint8_t width, uint8_t height)
{
	Game::init(width, height);
	LevelGenerator levelGenerator(width, height);
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