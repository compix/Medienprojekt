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


Game::Game(sf::RenderWindow* window, InputManager &inputManager, EventManager &events, SFMLDebugDraw* debugDraw)
	:m_timer(1.f), m_events(events), m_entities(events), m_systems(m_entities, events)
{
	m_window = window;

	m_shaderManager.updateScreenResolution(m_window->getSize());

	/*Setup PhysixSystem*/
	m_PhysixSystem = new PhysixSystem(6, 3, GameConstants::S_SCALE);
	m_PhysixSystem->SetDebugDrawer(debugDraw);
	BodyFactory::m_World = m_PhysixSystem->GetWorld();
	/*Setup PhysixSystem End*/


	m_layerManager = std::make_unique<LayerManager>();
	m_layerManager->createLayer(21, 21, 0);
	m_layerManager->createLayer(21, 21, -1);
	m_layerManager->configure(events);

	m_textureLoader = std::make_unique<TextureLoader>();
	m_textureLoader->loadAllFromJson("assets/json/textures.json");
	m_entityFactory = std::make_unique<EntityFactory>(m_entities, m_textureLoader.get(), m_PhysixSystem, m_layerManager.get(), &m_shaderManager);

	m_systems.add<InventorySystem>();
	m_systems.add<TimerSystem>();
	m_systems.add<BombSystem>(m_entityFactory.get());
	m_systems.add<DamageSystem>(m_layerManager.get());
	m_systems.add<DestructionSystem>();
	m_systems.add<ExplosionSystem>(m_entityFactory.get(), m_layerManager.get());
	m_systems.add<HealthSystem>();
	m_systems.add<DeathSystem>();
	m_systems.add<BodySystem>();
	m_systems.add<InputSystem>(inputManager);
	m_systems.add<InputHandleSystem>(m_entityFactory.get());
	m_systems.add<AnimationSystem>();
	m_systems.add<RenderSystem>(window, m_layerManager.get());
	m_systems.add<LightSystem>(window);
	m_systems.configure();

	LevelGenerator levelGenerator(m_entityFactory.get(), 21, 21);
	levelGenerator.generateRandomLevel();

	//m_systems.system<RenderSystem>()->setRenderTexture(&m_renderTexture);
	
	m_light.create(sf::Vector2f(35.f, 60.f), sf::Color::Yellow, 200.f, 360.f, 0.f);
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

	m_light.create(sf::Vector2f(m_mousePos.x, m_mousePos.y), sf::Color::Yellow, 200.f, 360.f, 0.f);
	m_light.setShader(m_shaderManager.getLightShader());

	Light light1(sf::Vector2f(35.f, 35.f), sf::Color::Yellow, 200.f, 360.f, 0.f);
	light1.setShader(m_shaderManager.getLightShader());
	Light light2(sf::Vector2f(150.f, 50.f), sf::Color::Yellow, 30.f, 360.f, 0.f);
	light2.setShader(m_shaderManager.getLightShader());

	m_window->draw(light1);
	m_window->draw(light2);
	m_window->draw(m_light);
}