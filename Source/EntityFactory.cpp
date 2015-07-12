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
#include "Events/PortalCreatedEvent.h"
#include "Events/ItemCreatedEvent.h"
#include "Events/BoostEffectCreatedEvent.h"
#include "Events/SmokeCreatedEvent.h"
#include "Utils/AssetManagement/TexturePacker.h"
#include "Utils/AssetManagement/AssetManager.h"
#include "Animation/AnimatorManager.h"
#include "Components/DirectionComponent.h"
#include "Components/DestructionDelayComponent.h"
#include "Components/EffectComponent.h"
#include "Components/DynamicComponent.h"
#include "Components/AIComponent.h"
#include <sstream>
#include "Components/PortalComponent.h"

EntityFactory::EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager)
	:m_physixSystem(physixSystem), m_layerManager(layerManager), m_shaderManager(shaderManager), m_systemManager(systemManager)
{
}

Entity EntityFactory::createPlayer(float x, float y, int playerIndex)
{
	Entity* entity = createEntity(); // createEntity() für Entities mit Body benutzen. Ist nicht schlimm, wenn das auch für welche ohne gemacht wrid.

	uint8_t cellX = x / GameConstants::CELL_WIDTH;
	uint8_t cellY = y / GameConstants::CELL_HEIGHT;

	TransformComponent transformComponent;
	transformComponent.x = x;
	transformComponent.y = y;
	transformComponent.scaleX = 2.f;
	transformComponent.scaleY = 2.f;

	entity->assign<TransformComponent>(transformComponent);
	entity->assign<SpriteComponent>();

	AnimationComponent animationComponent;
	entity->assign<AnimationComponent>(animationComponent);
	AnimatorManager::assignCharacterAnimator(*entity);

	entity->assign<DirectionComponent>();
	entity->assign<CellComponent>(cellX, cellY);

	uint16 isA = BodyFactory::PLAYER_1 + playerIndex;
	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateCircle(entity, 
						 x, y, 10.f,
						 b2_dynamicBody,
						 isA | BodyFactory::PLAYER,
						 ~BodyFactory::PLAYER_1 & ~BodyFactory::PLAYER_2 & ~BodyFactory::PLAYER_3 & ~BodyFactory::PLAYER_4 & ~BodyFactory::PLAYER);

	bodyComponent.body->SetFixedRotation(true);
	entity->assign<BodyComponent>(bodyComponent);

	entity->assign<InputComponent>();
	entity->assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity->assign<InventoryComponent>();
	entity->assign<DynamicComponent>();
	entity->assign<HealthComponent>(1);

	m_layerManager->add(*entity);

	return *entity;
}

entityx::Entity EntityFactory::createBlock(uint8_t cellX, uint8_t cellY)
{
	Entity* entity = createEntity();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	entity->assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("block");
	sprite.setOrigin(GameConstants::CELL_WIDTH*0.5f, GameConstants::CELL_HEIGHT*0.5f);
	entity->assign<SpriteComponent>(sprite);
	entity->assign<DestructionDelayComponent>(1);

	entity->assign<CellComponent>(cellX, cellY);
	entity->assign<HealthComponent>(1);

	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox(entity, 
												(float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f,
												(float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f,
												(float)GameConstants::CELL_WIDTH / 2.f,
												(float)GameConstants::CELL_HEIGHT / 2.f,
												b2_staticBody,
												BodyFactory::CollsionCategory::SOLID_BLOCK,
												~BodyFactory::CollsionCategory::NOTHING);

	entity->assign<BodyComponent>(bodyComponent);


	entity->assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity->assign<BlockComponent>();

	m_layerManager->add(*entity);

	return *entity;
}

entityx::Entity EntityFactory::createSolidBlock(uint8_t cellX, uint8_t cellY)
{
	Entity* entity = createEntity();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	entity->assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("solid_block");
	sprite.setOrigin(GameConstants::CELL_WIDTH*0.5f, GameConstants::CELL_HEIGHT*0.5f);
	entity->assign<SpriteComponent>(sprite);
	entity->assign<SolidBlockComponent>();

	entity->assign<CellComponent>(cellX, cellY);


	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox(entity, 
												(float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f,
												(float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f,
												(float)GameConstants::CELL_WIDTH/2.f,
												(float)GameConstants::CELL_HEIGHT/2.f,
												b2_staticBody,
												BodyFactory::CollsionCategory::SOLID_BLOCK,
												~BodyFactory::CollsionCategory::NOTHING);


	entity->assign<BodyComponent>(bodyComponent);

	entity->assign<LayerComponent>(GameConstants::MAIN_LAYER);

	m_layerManager->add(*entity);

	return *entity;
}

Entity EntityFactory::createBomb(uint8_t cellX, uint8_t cellY, Entity owner)
{
	Entity* entity = createEntity();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	transformComponent.scaleX = 1.5f;
	transformComponent.scaleY = 1.5f;

	entity->assign<TransformComponent>(transformComponent);
	auto texture = createSprite("bomb");
	entity->assign<SpriteComponent>(texture);
	entity->assign<BombComponent>(7, 0.06f);
	entity->assign<TimerComponent>(2.f);
	entity->assign<HealthComponent>(1);
	entity->assign<OwnerComponent>(owner);

	entity->assign<CellComponent>(cellX, cellY);

	entity->assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity->assign<DynamicComponent>();

	//Physix
	auto fixture = owner.component<BodyComponent>()->body->GetFixtureList();

	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox(entity, 
												transformComponent.x,
												transformComponent.y,
												texture.getLocalBounds().width-2,
												texture.getLocalBounds().height-2,
												b2_kinematicBody,
												BodyFactory::BOMB,
												~fixture->GetFilterData().categoryBits);

	bodyComponent.body->SetFixedRotation(true);
	/* Filter jonglieren, damit man nach einer Bombe mit dieser wieder Kollidiert */
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(PhysixSystem::toBox2D(texture.getLocalBounds().width), PhysixSystem::toBox2D(texture.getLocalBounds().height));
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	fixtureDef.filter.categoryBits = BodyFactory::BOMB_RADAR;
	fixtureDef.filter.maskBits = fixture->GetFilterData().categoryBits;
	fixtureDef.shape = &dynamicBox;
	bodyComponent.body->CreateFixture(&fixtureDef);

	entity->assign<BodyComponent>(bodyComponent);
	//Physix_END

	m_layerManager->add(*entity);

	sf::Sprite sprite = entity->component<SpriteComponent>()->sprite;

	GameGlobals::events->emit<BombCreatedEvent>(*entity, cellX, cellY, owner);
	return *entity;
}

Entity EntityFactory::createPortal(uint8_t cellX, uint8_t cellY, Entity owner)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	transformComponent.scaleX = 1.5f;
	transformComponent.scaleY = 1.5f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<OwnerComponent>(owner);
	entity.assign<PortalComponent>();
	entity.assign<TimerComponent>(GameConstants::PORTAL_TIMER_NOT_LINKED);

	entity.assign<CellComponent>(cellX, cellY);

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);

	auto manager = m_systemManager->system<ParticleSystem>()->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (emitter)
	{
		entity.assign<ParticleComponent>();
		entity.component<ParticleComponent>()->emitter = emitter;

		emitter->spawnTime(0.01f)
			.maxLifetime(5.f)
			.speedModifier(1.f)
			.velocityFunction([](float t) { t = t*2.f - 1.f; return sf::Vector2f(t*5.f, t*t*t*t*t*15.f); })
			.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
			.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(5, 5)))
			.spawnWidth(GameConstants::CELL_WIDTH)
			.spawnHeight(GameConstants::CELL_HEIGHT)
			.spawnDuration(100.f)
			.transparencyFunction(Gradient<float>(GradientType::REGRESS, 255, 0))
			.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(0, 150, 252), RGB(0, 255, 252)))
			.position((float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f, (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f
			);
	}

	m_layerManager->add(entity);

	GameGlobals::events->emit<PortalCreatedEvent>(entity, cellX, cellY, owner);
	return entity;
}

Entity EntityFactory::createExplosion(uint8_t cellX, uint8_t cellY, Direction direction, uint8_t range, float spreadTime)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;

	float width = GameConstants::CELL_WIDTH;
	float height = GameConstants::CELL_HEIGHT;

	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;

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
			.maxLifetime(0.25f)
			.gravityModifier(1.f)
			.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*100.f); })
			.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
			.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(20, 20)))
			.burstParticleNumber(10)
			.burstTime(0.5f)
			.spawnWidth(50 - 5)
			.spawnHeight(50)
			.spawnDuration(0.3)
			.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(5, 42, 252), RGB(255, 102, 0)));
	}

	entity.assign<SpreadComponent>(direction, range, spreadTime);
	entity.assign<ExplosionComponent>();
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<DestructionComponent>(0.55f);

	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(cellX, cellY);

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DynamicComponent>();

	m_layerManager->add(entity);

	GameGlobals::events->emit<ExplosionCreatedEvent>(entity, cellX, cellY, direction, range, spreadTime);
	return entity;
}

Entity EntityFactory::createExplosion(uint8_t cellX, uint8_t cellY, uint8_t range, float spreadTime)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(cellX, cellY);
	
	createExplosion(cellX, cellY, Direction::DOWN, range, spreadTime);
	createExplosion(cellX, cellY, Direction::UP, range, spreadTime);
	createExplosion(cellX, cellY, Direction::LEFT, range, spreadTime);
	createExplosion(cellX, cellY, Direction::RIGHT, range, spreadTime);

	entity.assign<ExplosionComponent>();
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DestructionComponent>(0.55f);
	entity.assign<DynamicComponent>();

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createFloor(uint8_t cellX, uint8_t cellY)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f + 7;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f + 5.f + 10;
	entity.assign<TransformComponent>(transformComponent);
	auto sprite = createSprite("floor");
	entity.assign<SpriteComponent>(sprite);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::FLOOR_LAYER);
	entity.assign<FloorComponent>();

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createSmoke(uint8_t cellX, uint8_t cellY)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DestructionComponent>(3.f);
	entity.assign<EffectComponent>();

	auto manager = m_systemManager->system<ParticleSystem>()->getManager("smoke");
	auto emitter = manager->spawnEmitter();

	if (emitter)
	{
		entity.assign<ParticleComponent>();
		entity.component<ParticleComponent>()->emitter = emitter;

		emitter->spawnTime(15.1f)
			.position(GameGlobals::window->getSize().x*0.5f, GameGlobals::window->getSize().y*0.5f)
			.maxLifetime(1.f)
			.speedModifier(5.5f)
			.velocityFunction([](float t){ return t < 0.5f ? sf::Vector2f(cosf(t), sinf(t)) : sf::Vector2f(sinf(t)*5.f, sinf(t)*5.f); })
			.spawnHeight(30)
			.spawnWidth(30)
			.burstParticleNumber(10)
			.burstTime(0.25f)
			.burstNumber(5)
			.blendMode(sf::BlendAlpha)
			.transparencyFunction([](float t) { return t < 0.5f ? 200.f*t : 200 - t * 200; })
			.sizeFunction(Gradient<sf::Vector2f>(GradientType::SMOOTH, sf::Vector2f(0, 0), sf::Vector2f(50, 50)));
	}

	m_layerManager->add(entity);

	GameGlobals::events->emit<SmokeCreatedEvent>(entity, cellX, cellY);
	return entity;
}

Entity EntityFactory::createBoostEffect(uint8_t cellX, uint8_t cellY, Entity target)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DestructionComponent>(1.5f);
	entity.assign<EffectComponent>();
	entity.assign<DynamicComponent>();

	auto manager = m_systemManager->system<ParticleSystem>()->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (emitter)
	{
		entity.assign<ParticleComponent>();
		entity.component<ParticleComponent>()->emitter = emitter;

		emitter->spawnTime(0.008f)
			.maxLifetime(0.9f)
			.speedModifier(5.f)
			.velocityFunction([](float t) { t = t*2.f - 1.f; return sf::Vector2f(t*5.f, t*t*t*t*t*15.f); })
			.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
			.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(5, 5)))
			.spawnWidth(15)
			.spawnHeight(50)
			.spawnDuration(0.6f)
			.transparencyFunction(Gradient<float>(GradientType::REGRESS, 255, 0))
			.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(0, 255, 252), RGB(42, 255, 0)))
			.follow(target);
	}

	m_layerManager->add(entity);

	GameGlobals::events->emit<BoostEffectCreatedEvent>(entity, cellX, cellY, target);
	return entity;
}

Entity EntityFactory::createItem(uint8_t cellX, uint8_t cellY, ItemType type)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f + 7.f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f + 15.f;
	transformComponent.scaleX = 0.70f;
	transformComponent.scaleY = 0.70f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<ItemComponent>(type);
	entity.assign<HealthComponent>(1);

	switch (type)
	{
	case ItemType::BOMB_CAP_BOOST: 
		entity.assign<SpriteComponent>(createSprite("bombCapBoost"));
		break;
	case ItemType::BOMB_KICK_SKILL:
		entity.assign<SpriteComponent>(createSprite("bomb_kick_skill"));
		break;
	case ItemType::SPEED_MULTIPLICATOR:
		entity.assign<SpriteComponent>(createSprite("speed_multiplicator"));
		break;
	default: break;
	}

	m_layerManager->add(entity);

	GameGlobals::events->emit<ItemCreatedEvent>(entity, cellX, cellY, type);
	return entity;
}

Entity* EntityFactory::createEntity()
{
	Entity entity = GameGlobals::entities->create();

	m_entityMap[entity.id()] = entity;

	return &m_entityMap[entity.id()];
}

void EntityFactory::destroyEntity(Entity entity)
{
	if (m_entityMap.count(entity.id())){
		m_entityMap.erase(entity.id());
	}
}

sf::Sprite EntityFactory::createSprite(const std::string& textureName)
{
	auto texture = GameGlobals::assetManager->getTexture(textureName);
	sf::Sprite sprite(*texture->get());
	sprite.setOrigin(texture->getRect().width*0.5f, texture->getRect().height*0.5f);
	sprite.setTextureRect(texture->getRect());

	return sprite;
}