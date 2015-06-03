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
#include "Events/ExplosionCreatedEvent.h"
#include "Utils/AssetManagement/TexturePacker.h"
#include "Utils/AssetManagement/AssetManager.h"
#include "Animation/AnimatorManager.h"
#include "Components/DirectionComponent.h"
#include "Components/DestructionDelayComponent.h"
#include "Components/SmokeComponent.h"

EntityFactory::EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager)
	:m_physixSystem(physixSystem), m_layerManager(layerManager), m_shaderManager(shaderManager), m_systemManager(systemManager)
{
}

Entity EntityFactory::createPlayer(float x, float y)
{
	Entity entity = GameGlobals::entities->create();

	uint8_t col = (x - GameConstants::CELL_WIDTH*0.5f) / GameConstants::CELL_WIDTH;
	uint8_t row = (y - GameConstants::CELL_HEIGHT*0.5f) / GameConstants::CELL_HEIGHT;
	TransformComponent transformComponent;
	transformComponent.x = x;
	transformComponent.y = y;
	transformComponent.scaleX = 2.f;
	transformComponent.scaleY = 2.f;

	entity.assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("char_idle");
	entity.assign<SpriteComponent>(sprite);

	AnimationComponent animationComponent;
	entity.assign<AnimationComponent>(animationComponent);
	AnimatorManager::assignCharacterAnimator(entity);

	entity.assign<DirectionComponent>();
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

entityx::Entity EntityFactory::createBlock(uint8_t row, uint8_t col)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("block");
	sprite.setOrigin(GameConstants::CELL_WIDTH*0.5f, GameConstants::CELL_HEIGHT*0.5f);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<DestructionDelayComponent>(1);

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

entityx::Entity EntityFactory::createSolidBlock(uint8_t row, uint8_t col)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float) GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float) GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("solid_block");
	sprite.setOrigin(GameConstants::CELL_WIDTH*0.5f, GameConstants::CELL_HEIGHT*0.5f);
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

Entity EntityFactory::createBomb(uint8_t row, uint8_t col, Entity owner)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;
	transformComponent.scaleX = 1.5f;
	transformComponent.scaleY = 1.5f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(createSprite("bomb"));
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

Entity EntityFactory::createExplosion(uint8_t row, uint8_t col, Direction direction, uint8_t range, float spreadTime)
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

	auto manager = m_systemManager->system<ParticleSystem>()->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (emitter)
	{
		entity.assign<ParticleComponent>();
		entity.component<ParticleComponent>()->emitter = emitter;

		emitter->spawnTime(0.0015f)
			.maxLifetime(0.3f)
			.gravityModifier(1.f)
			.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*100.f); })
			.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
			.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(20, 20)))
			.burstParticleNumber(10)
			.burstTime(0.5f)
			.spawnWidth(width-5)
			.spawnHeight(height)
			.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(5, 42, 252), RGB(255, 102, 0)));
	}

	entity.assign<SpreadComponent>(direction, range, spreadTime);
	entity.assign<ExplosionComponent>();
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(col, row);

	entity.assign<LayerComponent>(0);
		
	m_layerManager->add(entity);

	GameGlobals::events->emit<ExplosionCreatedEvent>(entity, col, row, direction, range, spreadTime);
	return entity;
}

Entity EntityFactory::createExplosion(uint8_t row, uint8_t col, uint8_t range, float spreadTime)
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

Entity EntityFactory::createFloor(uint8_t row, uint8_t col)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f + 7;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f + 5.f + 10;
	entity.assign<TransformComponent>(transformComponent);
	auto sprite = createSprite("floor");
	entity.assign<SpriteComponent>(sprite);

	entity.assign<CellComponent>(col, row);
	entity.assign<LayerComponent>(-1);
	entity.assign<FloorComponent>();

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createSmoke(uint8_t row, uint8_t col)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(col, row);
	entity.assign<LayerComponent>(0);
	entity.assign<DestructionComponent>(3.f);
	entity.assign<SmokeComponent>();

	auto manager = m_systemManager->system<ParticleSystem>()->getManager("smoke");
	auto emitter = manager->spawnEmitter();

	if (emitter)
	{
		entity.assign<ParticleComponent>();
		entity.component<ParticleComponent>()->emitter = emitter;

		emitter->spawnTime(10.0f)
			.maxLifetime(1.f)
			.speedModifier(0.5f)
			.velocityFunction(Gradient<sf::Vector2f>(GradientType::SMOOTH, sf::Vector2f(26, 26), sf::Vector2f(0, 0)))
			.spawnHeight(30)
			.spawnWidth(30)
			.burstParticleNumber(7)
			.burstTime(0.25f)
			.burstNumber(5)
			.sizeFunction(Gradient<sf::Vector2f>(GradientType::SMOOTH, sf::Vector2f(3, 3), sf::Vector2f(25, 25)))
			.colorFunction(Gradient<RGB>(GradientType::SMOOTH, RGB(46, 40, 35), RGB(100, 100, 100)));
	}

	m_layerManager->add(entity);

	return entity;
}

sf::Sprite EntityFactory::createSprite(const std::string& textureName)
{
	auto texture = GameGlobals::assetManager->getTexture(textureName);
	sf::Sprite sprite(*texture->get());
	sprite.setOrigin(texture->getRect().width*0.5f, texture->getRect().height*0.5f);
	sprite.setTextureRect(texture->getRect());

	return sprite;
}