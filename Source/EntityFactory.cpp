#include "EntityFactory.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/InputComponent.h"
#include "Components/BodyComponent.h"
#include <SFML/Graphics.hpp>
#include "BodyFactory.h"
#include "Components/CellComponent.h"
#include "Components/HealthComponent.h"
#include "GameConstants.h"
#include "Components/ExplosionComponent.h"
#include "Components/DamageDealerComponent.h"
#include "Components/SolidBlockComponent.h"
#include "Components/DestructionComponent.h"
#include "Components/BombComponent.h"
#include "Components/TimerComponent.h"
#include "Components/LayerComponent.h"
#include "Components/LinkComponent.h"
#include "Components/LightComponent.h"
#include "Utils/ShaderManager.h"
#include "Components/OwnerComponent.h"
#include "Components/InventoryComponent.h"
#include "GameGlobals.h"
#include "Components/FloorComponent.h"
#include "Components/BlockComponent.h"
#include "Utils/Colors.h"
#include "Utils/Functions.h"
#include "Graphics/ParticleEmitter.h"
#include "Components/ParticleComponent.h"
#include "Systems/ParticleSystem.h"
#include "Events/BombCreatedEvent.h"

EntityFactory::EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager)
	:m_physixSystem(physixSystem), m_layerManager(layerManager), m_shaderManager(shaderManager), m_systemManager(systemManager)
{
}

Entity EntityFactory::createPlayer(float x, float y)
{
	Entity entity = GameGlobals::entities->create();

	Texture& tex = GameGlobals::textures->get("char_idle");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	uint8_t col = (x - GameConstants::CELL_WIDTH*0.5f) / GameConstants::CELL_WIDTH;
	uint8_t row = (y - GameConstants::CELL_HEIGHT*0.5f) / GameConstants::CELL_HEIGHT;
	TransformComponent transformComponent;
	transformComponent.x = x;
	transformComponent.y = y;
	transformComponent.scaleX = 0.2f;
	transformComponent.scaleY = 0.2f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	AnimationComponent animationComponent;
	animationComponent.colCount = 8;
	animationComponent.rowCount = 3;
	animationComponent.frameCount = 20;
	animationComponent.frameDuration = 0.025f;
	animationComponent.playMode = PlayMode::LOOP;
	entity.assign<AnimationComponent>(animationComponent);

	entity.assign<CellComponent>(col, row);

	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateCircle(x, y, 10.f,
		b2_dynamicBody,
		BodyFactory::CollsionCategory::PLAYER,
		BodyFactory::CollsionCategory::SOLID_BLOCK);

	bodyComponent.body->SetFixedRotation(true);
	entity.assign<BodyComponent>(bodyComponent);

	InputComponent inputComponent;
	inputComponent.playerIndex = -1;
	entity.assign<InputComponent>(inputComponent);
	entity.assign<LayerComponent>(0);
	entity.assign<InventoryComponent>();

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createTestEntity2()
{
	sf::Sprite sprite;
	Texture& tex = GameGlobals::textures->get("char_death");
	sprite.setTexture(tex);

	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = 220.f;
	transformComponent.y = 180.f;
	transformComponent.scaleX = 0.2f;
	transformComponent.scaleY = 0.2f;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	AnimationComponent animationComponent;
	animationComponent.colCount = 8;
	animationComponent.rowCount = 5;
	animationComponent.frameCount = 40;
	animationComponent.frameDuration = 0.05f;
	animationComponent.playMode = PlayMode::LOOP_PING_PONG;
	entity.assign<AnimationComponent>(animationComponent);

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

entityx::Entity EntityFactory::createBlock(int row, int col)
{
	Entity entity = GameGlobals::entities->create();

	Texture& tex = GameGlobals::textures->get("block");
	sf::Sprite sprite;
	sprite.setTexture(tex);
	sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().x*0.5f);

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);
	
	entity.assign<CellComponent>(col, row);
	entity.assign<HealthComponent>(1);

	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox((float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f,
												(float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f,
												(float)GameConstants::CELL_WIDTH / 2.f,
												(float)GameConstants::CELL_HEIGHT / 2.f,
												b2_staticBody,
												BodyFactory::CollsionCategory::SOLID_BLOCK,
												~BodyFactory::CollsionCategory::NOTHING);


	entity.assign<BodyComponent>(bodyComponent);


	entity.assign<LayerComponent>(0);
	entity.assign<BlockComponent>();

	m_layerManager->add(entity);

	return entity;
}

entityx::Entity EntityFactory::createSolidBlock(int row, int col)
{
	Entity entity = GameGlobals::entities->create();

	Texture& tex = GameGlobals::textures->get("solid_block");
	sf::Sprite sprite;
	sprite.setTexture(tex);
	sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().x*0.5f);

	TransformComponent transformComponent;
	transformComponent.x = (float) GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float) GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<SolidBlockComponent>();

	entity.assign<CellComponent>(col, row);


	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox((float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f,
												(float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f,
												(float)GameConstants::CELL_WIDTH/2.f,
												(float)GameConstants::CELL_HEIGHT/2.f,
												b2_staticBody,
												BodyFactory::CollsionCategory::SOLID_BLOCK,
												~BodyFactory::CollsionCategory::NOTHING);


	entity.assign<BodyComponent>(bodyComponent);

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createBomb(int row, int col, Entity owner)
{
	Entity entity = GameGlobals::entities->create();

	Texture& tex = GameGlobals::textures->get("bomb");
	sf::Sprite sprite;
	sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().y*0.5f);
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = (float)tex.getSize().x * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)tex.getSize().y * row + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<BombComponent>(7, 0.06f);
	entity.assign<TimerComponent>(2.f);
	entity.assign<HealthComponent>(1);
	entity.assign<OwnerComponent>(owner);

	entity.assign<CellComponent>(col, row);

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	GameGlobals::events->emit<BombCreatedEvent>(entity, col, row, owner);
	return entity;
}

Entity EntityFactory::createExplosion(int row, int col, Direction direction, int range, float spreadTime)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;

	float width = GameConstants::CELL_WIDTH;
	float height = GameConstants::CELL_HEIGHT;

	transformComponent.x = GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;

	switch (direction)
	{
	case Direction::RIGHT:
		transformComponent.rotation = -90;
		break;
	case Direction::UP:
		transformComponent.rotation = 180.f;
		break;
	case Direction::LEFT:
		transformComponent.rotation = 90.f;
		break;
	case Direction::DOWN:
		break;
	}

	/*
	entity.assign<LightComponent>(sf::Vector2f(transformComponent.x, transformComponent.y), sf::Color(255, 140, 0), 100.f, 360.f, 0.f);
	auto lightComponent = entity.component<LightComponent>();
	lightComponent->light.setShader(m_shaderManager->getLightShader());
	lightComponent->light.setAttenuation(sf::Vector3f(200.f, 10.f, 0.2f));
	*/
	entity.assign<ParticleComponent>();

	auto manager = m_systemManager->system<ParticleSystem>()->getManager("light");

	auto emitter = manager->spawnEmitter();
	entity.component<ParticleComponent>()->emitter = emitter;

	emitter->spawnTime(0.0025f)
		.maxLifetime(0.3f)
		.gravityModifier(1.f)
		.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*100.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(10, 10), sf::Vector2f(20, 10)))
		.burstParticleNumber(10)
		.burstTime(0.5f)
		.spawnWidth(width)
		.spawnHeight(height)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(5, 42, 252), RGB(255, 102, 0)));

	entity.assign<SpreadComponent>(direction, range, spreadTime);
	entity.assign<ExplosionComponent>();
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(col, row);

	entity.assign<LayerComponent>(0);
		
	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createExplosion(int row, int col, int range, float spreadTime)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(col, row);
	
	LinkComponent linkComponent;
	linkComponent.links.push_back(createExplosion(row, col, Direction::DOWN,  range, spreadTime));
	linkComponent.links.push_back(createExplosion(row, col, Direction::UP,    range, spreadTime));
	linkComponent.links.push_back(createExplosion(row, col, Direction::LEFT,  range, spreadTime));
	linkComponent.links.push_back(createExplosion(row, col, Direction::RIGHT, range, spreadTime));

	entity.assign<ExplosionComponent>();
	entity.assign<LinkComponent>(linkComponent);
	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createFloor(int row, int col)
{
	Entity entity = GameGlobals::entities->create();

	Texture& tex = GameGlobals::textures->get("floor");
	sf::Sprite sprite;
	sprite.setTexture(tex);
	sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().x*0.5f);

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f + 5.f;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	entity.assign<CellComponent>(col, row);
	entity.assign<LayerComponent>(-1);
	entity.assign<FloorComponent>();

	m_layerManager->add(entity);

	return entity;
}