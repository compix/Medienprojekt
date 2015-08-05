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
#include "Utils/PathFinding/PathEngine.h"
#include "Components/InventoryComponent.h"
#include "Components/CellComponent.h"
#include "Systems/AISystem.h"
#include "Systems/SoundSystem.h"
#include "Systems/MusicSystem.h"

#include "Systems/BombKickSystem.h"
#include "Systems/PortalSystem.h"
#include "Utils/InputManager.h"
#include "Systems/ChatRenderSystem.h"

#include "Components/InputComponent.h"
#include "Components/FreeSlotComponent.h"
#include "Components/LocalInputComponent.h"
#include "Components/AIComponent.h"


Game::Game()
	:m_timer(1.f), m_debugDraw(*GameGlobals::window), m_PhysixSystem(nullptr)
{
	GameGlobals::gameView = &m_view;
	GameGlobals::engine = &m_engine;
}

Game::~Game() {
	GameGlobals::entityFactory = nullptr;
	m_engine.removeAllEntities();
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
	m_layerManager->addedToEngine(&m_engine);

	m_pathEngine = std::make_unique<PathEngine>(m_layerManager.get());

	m_entityFactory = std::make_unique<EntityFactory>(m_PhysixSystem, m_layerManager.get(), &m_shaderManager, &m_engine);
	GameGlobals::entityFactory = m_entityFactory.get();

	addSystems();

	//m_light.create(sf::Vector2f(35.f, 60.f), sf::Color::Yellow, 200.f, 360.f, 0.f);

	auto particleSystem = m_engine.getSystem<ParticleSystem>();
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
}

void Game::update(float dt)
{
	if (!initialized)
		return;

	GameGlobals::input->update();
	m_PhysixSystem->Update(dt);
	m_engine.update(dt);
	//m_PhysixSystem->DrawDebug();
	m_layerManager->update();

	//m_light.create(sf::Vector2f(m_mousePos.x, m_mousePos.y), sf::Color::Yellow, 200.f, 360.f, 0.f);
	//m_light.setShader(m_shaderManager.getLightShader());

	GameGlobals::window->draw(m_light);
	GameGlobals::window->draw(*m_particleEmitter);

	//auto player = *m_entities.entities_with_components<InventoryComponent>().begin();
	//auto cell = player->get<CellComponent>();
}

void Game::refreshView()
{
	float gameW = GameGlobals::game->getWidth() * GameConstants::CELL_WIDTH;
	float gameH = GameGlobals::game->getHeight() * GameConstants::CELL_HEIGHT;
	float screenW = GameGlobals::window->getSize().x;
	float screenH = GameGlobals::window->getSize().y;
	float screenRatio = screenW / screenH;
	float viewRatio = gameW / gameH;
	float scaleFactor;
	if (viewRatio > screenRatio)
		scaleFactor = gameW / screenW;
	else
		scaleFactor = gameH / screenH;
	m_view.reset(sf::FloatRect(0, 0, screenW * scaleFactor, screenH * scaleFactor));
	m_view.setCenter(gameW*0.5f, gameH*0.5f);
	m_shaderManager.updateScreenResolution(GameGlobals::window->getSize());
}

void LocalGame::addSystems()
{
	//fixme: delete
	m_engine.addSystem(new PortalSystem(m_layerManager.get()));
	m_engine.addSystem(new BodySystem());
	m_engine.addSystem(new SoundSystem());
	m_engine.addSystem(new MusicSystem());
	m_engine.addSystem(new InventorySystem());
	m_engine.addSystem(new TimerSystem());
	m_engine.addSystem(new BombSystem());
	m_engine.addSystem(new DamageSystem(m_layerManager.get()));
	m_engine.addSystem(new DestructionSystem());
	m_engine.addSystem(new ExplosionSystem(m_layerManager.get()));
	m_engine.addSystem(new BombKickSystem(m_layerManager.get()));
	m_engine.addSystem(new HealthSystem());
	m_engine.addSystem(new DeathSystem());
	m_engine.addSystem(new InputSystem());
	m_engine.addSystem(new AISystem(m_pathEngine.get()));
	m_engine.addSystem(new InputHandleSystem());
	m_engine.addSystem(new AnimationSystem());
	m_engine.addSystem(new RenderSystem(m_layerManager.get()));
	m_engine.addSystem(new ParticleSystem());
	m_engine.addSystem(new LightSystem());
	m_engine.addSystem(new ItemSystem(m_layerManager.get()));
	m_engine.addSystem(new ParticleSpawnSystem(m_engine.getSystem<ParticleSystem>(), m_layerManager.get()));
	m_engine.addSystem(new ChatRenderSystem());
}

void LocalGame::initPlayers(const vector<CreateGamePlayerInfo> &players)
{
	m_numPlayers = players.size();
	for (int i = 0; i < m_numPlayers; i++)
		m_playerTypes[i] = players[i].type;
}

void LocalGame::resetEntities()
{
	m_engine.removeAllEntities();
	LevelGenerator levelGenerator(m_width, m_height);
	levelGenerator.generateRandomLevel();

	m_pathEngine->getGraph()->init();

	int i = 0;
	for(Entity *entity: *m_engine.getEntitiesFor(Family::all<InputComponent>().get()))
	{
		if (i == m_numPlayers)
			m_engine.removeEntity(entity);
		else
		{
			switch (m_playerTypes[i])
			{
			case CreateGamePlayerType::LOCAL: entity->assign<LocalInputComponent>(i); break;
			case CreateGamePlayerType::COMPUTER: entity->assign<AIComponent>(); break;
			case CreateGamePlayerType::CLIENT: entity->assign<FreeSlotComponent>(); break;
			}
			i++;
		}
	}
	GameGlobals::events->startGame.emit();
}

void ClientGame::addSystems()
{
	//fixme: delete
	m_engine.addSystem(new InputSystem());
//	m_engine.addSystem(new ClientInputHandleSystem()); // fixme
	m_engine.addSystem(new AnimationSystem());
	m_engine.addSystem(new RenderSystem(m_layerManager.get()));
	m_engine.addSystem(new ParticleSystem());
	m_engine.addSystem(new LightSystem());
	m_engine.addSystem(new ParticleSpawnSystem(m_engine.getSystem<ParticleSystem>(), m_layerManager.get()));
}