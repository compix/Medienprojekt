#include "Game.h"
#include "Systems/AnimationSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/BodySystem.h"
#include "Systems/RenderSystem.h"
#include "Utils/LevelGenerator.h"
#include "Utils/make_unique.h"
#include "BodyFactory.h"


using namespace std;

Game::Game(sf::RenderWindow* window, InputManager &inputManager, SFMLDebugDraw* debugDraw)
{

	/*Setup PhysixSystem*/
	m_PhysixSystem = new PhysixSystem(6, 3, S_SCALE);
	m_PhysixSystem->SetDebugDrawer(debugDraw);
	BodyFactory::m_World = m_PhysixSystem->GetWorld();
	/*Setup PhysixSystem End*/

	m_layerManager = make_unique<LayerManager>();

	systems.add<BodySystem>();
	systems.add<InputSystem>(inputManager);
	systems.add<AnimationSystem>();
	systems.add<RenderSystem>(window, m_layerManager.get());
	systems.configure();

	m_textureLoader = make_unique<TextureLoader>();
	m_textureLoader->loadAllFromJson("assets/json/textures.json");

	m_entityFactory = make_unique<EntityFactory>(this, m_textureLoader.get(), m_PhysixSystem, m_layerManager.get());

// 	Entity entity = m_pEntityFactory->createTestEntity1(100.f, 100.f);
// 	Entity entity2 = m_pEntityFactory->createTestEntity2();

	//ComponentHandle<TransformComponent> t = entity.component<TransformComponent>();

	EntityLayer& entityLayer = m_layerManager->createLayer(0, true);

	LevelGenerator levelGenerator(m_entityFactory.get(), 21, 21);
	levelGenerator.generateRandomLevel();


// 	entityLayer.add(entity2);
// 	entityLayer.add(entity);

	//createTestLevel(entityLayer);

	m_layerManager->sortLayers(DepthComparator());
}

Game::~Game() { 
	delete m_PhysixSystem;
}

void Game::update(TimeDelta dt)
{
	m_PhysixSystem->Update(dt);
	systems.update_all(dt);
	m_PhysixSystem->DrawDebug();
}

void Game::createTestLevel(EntityLayer& layer)
{
	int blockWidth = 25;
	int blockHeight = 25;

	for (int x = 0; x < 21; x++)
	{
		for (int y = 0; y < 21; y++)
		{
			Entity e;

			if (x == 0 || x == 20 || y == 0 || y == 20 || (x % 2 == 0 && y % 2 == 0))
			{
				e = m_entityFactory->createSolidBlock(x*blockWidth, y * blockHeight);
				layer.add(e);
			}
			else
			{
				//e = m_pEntityFactory->createBlock(x*blockWidth, y*blockHeight);
				//layer.add(e);
			}
		}
	}
}
