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
#include "Components/InventoryComponent.h"
#include "Components/CellComponent.h"
#include "Systems/AISystem.h"
#include "Systems/SoundSystem.h"
#include "Systems/MusicSystem.h"
#include "Events/StartGameEvent.h"
#include "Systems/BombKickSystem.h"
#include "Systems/PortalSystem.h"
#include "Utils/InputManager.h"
#include "Systems/ChatRenderSystem.h"
#include "Events/CreateGameEvent.h"
#include "Events/GameOverEvent.h"
#include "Components/InputComponent.h"
#include "Components/FreeSlotComponent.h"
#include "Components/LocalInputComponent.h"
#include "Components/AIComponent.h"
#include "Events/ResetGameEvent.h"
#include "Systems/JumpSystem.h"
#include "Systems/HoldingSystem.h"
#include "Systems/BlinkSystem.h"
#include "Systems/AfterimageSystem.h"
#include "Graphics/ParticleEffects.h"

void fitViewInto(sf::View &view, float sourceW, float sourceH, float destW, float destH)
{
	float screenRatio = destW / destH;
	float viewRatio = sourceW / sourceH;
	float scaleFactor;
	if (viewRatio > screenRatio)
		scaleFactor = sourceW / destW;
	else
		scaleFactor = sourceH / destH;
	view.reset(sf::FloatRect(0, 0, destW * scaleFactor, destH * scaleFactor));
	view.setCenter(sourceW*0.5f, sourceH*0.5f);
}

Game::Game()
	:m_timer(1.f), m_entities(*GameGlobals::events), m_systems(m_entities, *GameGlobals::events), m_debugDraw(*GameGlobals::window), m_PhysixSystem(nullptr)
{
	GameGlobals::entities = &m_entities;

	GameGlobals::gameView = &m_view;
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
	if (!isClient()) {
		m_PhysixSystem = new PhysixSystem(8, 8, GameConstants::S_SCALE);
		m_PhysixSystem->setContactListener(&listener);
		m_PhysixSystem->SetDebugDrawer(&m_debugDraw);
		BodyFactory::m_World = m_PhysixSystem->GetWorld();
	}
	/*Setup PhysixSystem End*/


	m_layerManager = std::make_unique<LayerManager>();
	m_layerManager->createLayer(width, height, GameConstants::MAIN_LAYER);
	m_layerManager->createLayer(width, height, GameConstants::FLOOR_LAYER);
	m_layerManager->createLayer(width, height, GameConstants::JUMP_LAYER);
	m_layerManager->configure(*GameGlobals::events);

	

	m_entityFactory = std::make_unique<EntityFactory>(isClient(), m_layerManager.get(), &m_shaderManager, &m_systems);
	GameGlobals::entityFactory = m_entityFactory.get();

	addSystems();
	m_systems.configure();

	//m_light.create(sf::Vector2f(35.f, 60.f), sf::Color::Yellow, 200.f, 360.f, 0.f);

	auto particleSystem = m_systems.system<ParticleSystem>();
	m_particleEmitter = particleSystem->getManager("light")->spawnEmitter();

	// This Particle Emitter is just for tests.
	m_particleEmitter->
		position(GameGlobals::window->getSize().x*0.5f, GameGlobals::window->getSize().y*0.5f)
		.spawnTime(0.0015f)
		.maxLifetime(0.3f)
		.gravityModifier(1.f)
		.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*100.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(20, 20)))
		.burstParticleNumber(10)
		.burstTime(0.5f)
		.spawnWidth(50 - 5)
		.spawnHeight(50)
		.spawnDuration(0.3f)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(0, 252, 255), RGB(0, 84, 255)));

	ParticleEffects::init(m_systems.system<ParticleSystem>().get());
	initialized = true;
}

void Game::update(TimeDelta dt)
{
	if (!initialized)
		return;

	GameGlobals::input->update();
	if (m_PhysixSystem)
		m_PhysixSystem->Update(dt);

	for (auto& system : m_orderedSystems)
		system->update(m_entities, *GameGlobals::events, dt);


	//m_PhysixSystem->DrawDebug();
	m_layerManager->update();

	//m_light.create(sf::Vector2f(m_mousePos.x, m_mousePos.y), sf::Color::Yellow, 200.f, 360.f, 0.f);
	//m_light.setShader(m_shaderManager.getLightShader());

	//GameGlobals::window->draw(m_light);
	GameGlobals::window->draw(*m_particleEmitter);

	//m_systems.system<AISystem>()->visualize();
}

void Game::refreshView()
{
	float gameW = static_cast<float>(GameGlobals::game->getWidth() * GameConstants::CELL_WIDTH);
	float gameH = static_cast<float>(GameGlobals::game->getHeight() * GameConstants::CELL_HEIGHT);
	float screenW = static_cast<float>(GameGlobals::window->getSize().x);
	float screenH = static_cast<float>(GameGlobals::window->getSize().y);
	fitViewInto(m_view, gameW, gameH, screenW, screenH);
	m_shaderManager.updateScreenResolution(GameGlobals::window->getSize());
}

LocalGame::LocalGame()
{
	GameGlobals::events->subscribe<GameOverEvent>(*this);
}

LocalGame::~LocalGame()
{
	GameGlobals::events->unsubscribe<GameOverEvent>(*this);
}

void LocalGame::addSystems()
{
	addSystem<BodySystem>();
	addSystem<SoundSystem>();
	addSystem<MusicSystem>();
	addSystem<InventorySystem>();
	addSystem<ItemSystem>(m_layerManager.get());
	addSystem<TimerSystem>();
	addSystem<BombSystem>();
	addSystem<DamageSystem>(m_layerManager.get());
	addSystem<HoldingSystem>();
	addSystem<DestructionSystem>();
	addSystem<ExplosionSystem>(m_layerManager.get());
	addSystem<BlinkSystem>(m_layerManager.get());
	addSystem<PortalSystem>(m_layerManager.get());
	addSystem<JumpSystem>(m_layerManager.get());
	addSystem<BombKickSystem>(m_layerManager.get());
	addSystem<AfterimageSystem>();
	addSystem<HealthSystem>();
	addSystem<DeathSystem>();
	addSystem<InputSystem>();
	addSystem<InputHandleSystem>(m_layerManager.get());
	addSystem<AnimationSystem>();
	addSystem<RenderSystem>(m_layerManager.get());
	addSystem<ParticleSystem>();
	addSystem<LightSystem>();	
	addSystem<ParticleSpawnSystem>(m_systems.system<ParticleSystem>().get(), m_layerManager.get());
	addSystem<ChatRenderSystem>();
	addSystem<AISystem>(m_layerManager.get());
}

void LocalGame::initPlayers(const vector<CreateGamePlayerInfo> &players)
{
	m_numPlayers = players.size();
	for (int i = 0; i < m_numPlayers; i++)
		m_playerTypes[i] = players[i].type;
}

void LocalGame::resetEntities()
{
	m_entities.reset();
	m_layerManager->reset();
	GameGlobals::events->emit<ResetGameEvent>();

	if (m_systems.system<AISystem>())
		m_systems.system<AISystem>()->reset();

	LevelGenerator levelGenerator(DefaultConfig(m_width, m_height));
	levelGenerator.generateRandomLevel();

	uint8_t i = 0;
	uint8_t aiId = 0;
	ComponentHandle<InputComponent> input;
	for (Entity entity : GameGlobals::entities->entities_with_components(input))
	{
		if (i == m_numPlayers)
			entity.destroy();
		else
		{
			switch (m_playerTypes[i])
			{
			case CreateGamePlayerType::LOCAL: entity.assign<LocalInputComponent>(i); break;
			case CreateGamePlayerType::COMPUTER: m_entityFactory->initAI(entity, aiId++); break;
			case CreateGamePlayerType::CLIENT: entity.assign<FreeSlotComponent>(); break;
			}
			i++;
		}
	}

	m_systems.system<AISystem>()->init();
	GameGlobals::events->emit<StartGameEvent>();
}

void LocalGame::update(TimeDelta dt)
{
	if (m_resetTime > 0)
	{
		m_resetTime -= dt;
		if (m_resetTime <= 0)
		{
			m_resetTime = 0;
			resetEntities();
		}
	}
	Game::update(dt);
}

void LocalGame::receive(const GameOverEvent& evt)
{
	m_resetTime = GameConstants::RESET_GAME_COUNTDOWN;
}

ClientGame::ClientGame()
{
	GameGlobals::events->subscribe<ResetGameEvent>(*this);
}

ClientGame::~ClientGame()
{
	GameGlobals::events->unsubscribe<ResetGameEvent>(*this);
}

void ClientGame::receive(const ResetGameEvent& evt)
{
	m_entities.reset();
	m_layerManager->reset();
}

void ClientGame::addSystems()
{
	addSystem<SoundSystem>();
	addSystem<MusicSystem>();
	addSystem<BlinkSystem>(m_layerManager.get());
	addSystem<TimerSystem>();
	addSystem<AfterimageSystem>();
	addSystem<InputSystem>();
//	addSystem<ClientInputHandleSystem>(); // fixme
	addSystem<AnimationSystem>();
	addSystem<RenderSystem>(m_layerManager.get());
	addSystem<ParticleSystem>();
	addSystem<LightSystem>();
	addSystem<ParticleSpawnSystem>(m_systems.system<ParticleSystem>().get(), m_layerManager.get());
	addSystem<ChatRenderSystem>();
}
