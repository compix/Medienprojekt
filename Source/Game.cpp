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


Game::Game()
	:m_timer(1.f), m_entities(*GameGlobals::events), m_systems(m_entities, *GameGlobals::events), m_debugDraw(*GameGlobals::window)
{
	GameGlobals::entities = &m_entities;

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
	m_layerManager->createLayer(21, 21, 0);
	m_layerManager->createLayer(21, 21, -1);
	m_layerManager->configure(*GameGlobals::events);

	m_entityFactory = std::make_unique<EntityFactory>(m_PhysixSystem, m_layerManager.get(), &m_shaderManager);
	GameGlobals::entityFactory = m_entityFactory.get();

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
	m_systems.add<LightSystem>();
	m_systems.configure();

	LevelGenerator levelGenerator(21, 21);
	levelGenerator.generateRandomLevel();
	
	m_light.create(sf::Vector2f(35.f, 60.f), sf::Color::Yellow, 200.f, 360.f, 0.f);

	m_particleEmitter.setTexture(GameGlobals::textures->get("light"));
	m_particleEmitter.setPosition(GameGlobals::window->getSize().x*0.5f, GameGlobals::window->getSize().y*0.5f);

	m_particleEmitter.spawnTime(0.003f)
		.maxParticles(10000)
		.maxLifetime(5.f)
		.gravityModifier(5.f)
		.velocityFunction([](float t) { return sf::Vector2f(t, sinf(t)*100.f); })
		.angularVelocityFunction([](float t) { return t*t*0.1f; })
		.sizeFunction([](float t) { return sf::Vector2f(15 - t*t*50.f, 15 - t*t*t*20.f); })
		.colorFunction([](float t) { return sf::Color(0.f, Math::smootherstep(234, 23, t)*255.f, 255.f - Math::regress(t) * 189, t < 0.1 ? 15.f : 255 - t * 255); });
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

	m_particleEmitter.update(dt);

	m_light.create(sf::Vector2f(m_mousePos.x, m_mousePos.y), sf::Color::Yellow, 200.f, 360.f, 0.f);
	m_light.setShader(m_shaderManager.getLightShader());

	Light light1(sf::Vector2f(35.f, 35.f), sf::Color::Yellow, 200.f, 360.f, 0.f);
	light1.setShader(m_shaderManager.getLightShader());
	Light light2(sf::Vector2f(150.f, 50.f), sf::Color::Yellow, 30.f, 360.f, 0.f);
	light2.setShader(m_shaderManager.getLightShader());

	GameGlobals::window->draw(m_particleEmitter);
	GameGlobals::window->draw(light1);
	GameGlobals::window->draw(light2);
	GameGlobals::window->draw(m_light);
	
}