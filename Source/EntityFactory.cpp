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
#include "Graphics/ParticleEmitter.h"
#include "Components/ParticleComponent.h"
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
#include "Components/PortalComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/ColorComponent.h"
#include "Components/PortalMarkerComponent.h"
#include "Components/AfterimageComponent.h"
#include "Components/ExplosionStopComponent.h"
#include "Components/AIComponent.h"
#include "Graphics/ParticleEffects.h"
#include "Components/NoNetComponent.h"
#include "Events/LavaCreatedEvent.h"
#include "Components/LavaComponent.h"
#include "Events/LavaSpotMarkedEvent.h"
#include "Components/MarkedLavaSpotComponent.h"
#include "Components/SoundComponent.h"

EntityFactory::EntityFactory(bool isClient, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager)
	:m_isClient(isClient), m_layerManager(layerManager), m_shaderManager(shaderManager), m_systemManager(systemManager)
{
}

Entity EntityFactory::createPlayer(float x, float y, uint8_t playerIndex)
{
	Entity entity = GameGlobals::entities->create();

	static int playerId = 0;
	++playerId;

	if (playerId == 5)
		playerId = 1;

	uint8_t cellX = static_cast<uint8_t>(x / GameConstants::CELL_WIDTH);
	uint8_t cellY = static_cast<uint8_t>(y / GameConstants::CELL_HEIGHT);

	TransformComponent transformComponent;
	transformComponent.x = x;
	transformComponent.y = y;
	transformComponent.scaleX = 1.f;
	transformComponent.scaleY = 1.f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>();
	//entity.assign<SoundComponent>(0.5f, "walk", GameConstants::SOUND_VOLUME * 0.05f);

	AnimationComponent animationComponent;
	entity.assign<AnimationComponent>(animationComponent);
	AnimatorManager::assignCharacterAnimator(entity, playerIndex);

	entity.assign<DirectionComponent>();
	entity.assign<CellComponent>(cellX, cellY);

	uint16 isA = BodyFactory::PLAYER_1;
	switch (playerId)
	{
	case 1:
		isA = BodyFactory::PLAYER_1;
		entity.assign<ColorComponent>(RGB::Blue);
		break;
	case 2:
		isA = BodyFactory::PLAYER_2;
		entity.assign<ColorComponent>(RGB::Red);
		break;
	case 3:
		isA = BodyFactory::PLAYER_3;
		entity.assign<ColorComponent>(RGB::Green);
		break;
	case 4:
		isA = BodyFactory::PLAYER_4;
		entity.assign<ColorComponent>(RGB::Yellow);
		break;
	default:
		isA = BodyFactory::PLAYER_1;
		break;
	}

	if (!m_isClient)
	{
		BodyComponent bodyComponent;
		bodyComponent.body = BodyFactory::CreateCircle(entity,
			x, y, 10.f,
			b2_dynamicBody,
			isA | BodyFactory::PLAYER,
			~BodyFactory::PLAYER_1 & ~BodyFactory::PLAYER_2 & ~BodyFactory::PLAYER_3 & ~BodyFactory::PLAYER_4 & ~BodyFactory::PLAYER);

		bodyComponent.body->SetFixedRotation(true);
		entity.assign<BodyComponent>(bodyComponent);
	}
	entity.assign<InputComponent>();
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DynamicComponent>();
	entity.assign<InventoryComponent>();
	entity.assign<HealthComponent>(1);
	entity.assign<PlayerComponent>(playerIndex);

	m_layerManager->add(entity);

	return entity;
}

entityx::Entity EntityFactory::createBlock(uint8_t cellX, uint8_t cellY)
{
	Entity entity = GameGlobals::entities->create();;

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH * 0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT * 0.5f;
	entity.assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("block");
	sprite.setOrigin(GameConstants::CELL_WIDTH*0.5f, GameConstants::CELL_HEIGHT*0.5f);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<DestructionDelayComponent>(1.f);

	entity.assign<ExplosionStopComponent>();
	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<HealthComponent>(1);

	if (!m_isClient)
	{
		BodyComponent bodyComponent;
		bodyComponent.body = BodyFactory::CreateBox(entity,
			GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH * 0.5f,
			GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT * 0.5f,
			GameConstants::CELL_WIDTH / 2.f,
			GameConstants::CELL_HEIGHT / 2.f,
			b2_staticBody,
			BodyFactory::CollsionCategory::SOLID_BLOCK,
			~BodyFactory::CollsionCategory::NOTHING);

		entity.assign<BodyComponent>(bodyComponent);
	}

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<BlockComponent>();

	m_layerManager->add(entity);

	return entity;
}

entityx::Entity EntityFactory::createSolidBlock(uint8_t cellX, uint8_t cellY)
{
	Entity entity = GameGlobals::entities->create();;

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);
	sf::Sprite sprite = createSprite("solid_block");
	sprite.setOrigin(GameConstants::CELL_WIDTH*0.5f, GameConstants::CELL_HEIGHT*0.5f);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<SolidBlockComponent>();
	entity.assign<ExplosionStopComponent>();
	entity.assign<CellComponent>(cellX, cellY);

	if (!m_isClient)
	{
		BodyComponent bodyComponent;
		bodyComponent.body = BodyFactory::CreateBox(entity,
			GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f,
			GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f,
			GameConstants::CELL_WIDTH / 2.f,
			GameConstants::CELL_HEIGHT / 2.f,
			b2_staticBody,
			BodyFactory::CollsionCategory::SOLID_BLOCK,
			~BodyFactory::CollsionCategory::NOTHING);


		entity.assign<BodyComponent>(bodyComponent);
	}

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createBomb(uint8_t cellX, uint8_t cellY, Entity owner, BombType type)
{
	Entity entity = GameGlobals::entities->create();;

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH * 0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT * 0.5f;
	transformComponent.scaleX = 0.75f;
	transformComponent.scaleY = 0.75f;

	entity.assign<TransformComponent>(transformComponent);
	switch (type)
	{
	case BombType::GHOST: entity.assign<SpriteComponent>(createSprite("ghost_bomb")); break;
	case BombType::LIGHTNING: entity.assign<SpriteComponent>(createSprite("lightning_bomb")); break;
	default: entity.assign<SpriteComponent>(createSprite("bomb")); break;
	}
	assert(owner.has_component<InventoryComponent>());
	entity.assign<BombComponent>(owner.component<InventoryComponent>()->getExplosionRange(), GameConstants::EXPLOSION_SPREAD_TIME, type);
	entity.assign<TimerComponent>(GameConstants::EXPLOSION_TIME);
	entity.assign<HealthComponent>(1);
	entity.assign<OwnerComponent>(owner);

	entity.assign<CellComponent>(cellX, cellY);

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DynamicComponent>();


	uint16 filter = 0;
	auto entitiesOnTarget = m_layerManager->getEntitiesWithOneComponent<PlayerComponent, AIComponent>(GameConstants::MAIN_LAYER, cellX, cellY);
	for (auto e : entitiesOnTarget)
	{
		if (e.has_component<BodyComponent>()){
			filter |= e.component<BodyComponent>()->body->GetFixtureList()->GetFilterData().categoryBits;
		}
	}

	filter = ~filter;

	//Physix
	if (!m_isClient)
	{
		auto fixture = owner.component<BodyComponent>()->body->GetFixtureList();

		BodyComponent bodyComponent;
		bodyComponent.body = BodyFactory::CreateBox(entity,
			transformComponent.x,
			transformComponent.y,
			GameConstants::CELL_WIDTH * 0.5f,
			GameConstants::CELL_HEIGHT* 0.5f,
			b2_kinematicBody,
			BodyFactory::BOMB,
			filter);

		bodyComponent.body->SetFixedRotation(true);
		/* Filter jonglieren, damit man nach einer Bombe mit dieser wieder Kollidiert */
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(PhysixSystem::toBox2D(GameConstants::CELL_WIDTH * 0.5f), PhysixSystem::toBox2D(GameConstants::CELL_HEIGHT * 0.5f));
		b2FixtureDef fixtureDef;
		fixtureDef.isSensor = true;
		fixtureDef.filter.categoryBits = BodyFactory::BOMB_RADAR;
		fixtureDef.filter.maskBits = fixture->GetFilterData().categoryBits;
		fixtureDef.shape = &dynamicBox;
		bodyComponent.body->CreateFixture(&fixtureDef);

		entity.assign<BodyComponent>(bodyComponent);
	}
	//Physix_END

	// Don't teleport if placed on a portal
	auto portal = m_layerManager->getEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, cellX, cellY);
	if (portal)
		entity.assign<PortalMarkerComponent>(portal.id());

	m_layerManager->add(entity);

	GameGlobals::events->emit<BombCreatedEvent>(entity, cellX, cellY, owner, type);
	return entity;
}

Entity EntityFactory::createPortal(uint8_t cellX, uint8_t cellY, Entity owner, bool linked)
{
	RGB ownerColor;
	if (owner.has_component<ColorComponent>())
		ownerColor = owner.component<ColorComponent>()->color;

	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH * 0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT * 0.5f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<OwnerComponent>(owner);
	entity.assign<PortalComponent>();
	entity.assign<TimerComponent>(linked ? GameConstants::PORTAL_TIMER_LINKED : GameConstants::PORTAL_TIMER_NOT_LINKED);

	entity.assign<CellComponent>(cellX, cellY);

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);

	auto emitter = ParticleEffects::portal(ownerColor);

	if (emitter)
		entity.assign<ParticleComponent>(emitter);

	GameGlobals::events->emit<PortalCreatedEvent>(entity, cellX, cellY, owner);
	m_layerManager->add(entity);
	
	return entity;
}

Entity EntityFactory::createAfterimage(int cellX, int cellY, float posX, float posY, sf::Sprite sprite, float time)
{
	Entity entity = GameGlobals::entities->create();
	TransformComponent transformComponent;
	transformComponent.x = posX;
	transformComponent.y = posY;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<TimerComponent>(time);
	entity.assign<AfterimageComponent>(time);
	entity.assign<NoNetComponent>();
	m_layerManager->add(entity);
	return entity;
}

void EntityFactory::createExplosion(uint8_t cellX, uint8_t cellY, uint8_t range, float spreadTime, BombType bombType)
{
	entityx::Entity explosions[4];
	for (uint8_t i = 0; i < 4; ++i)
	{
		Direction direction = static_cast<Direction>(i);
		explosions[i] = createExplosion(cellX, cellY, direction, range, spreadTime, bombType);
	}

	// Don't teleport if placed on a portal
	auto portal = m_layerManager->getEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, cellX, cellY);
	if (portal)
	{
		for (auto e : explosions)
			e.assign<PortalMarkerComponent>(portal.id());
	}
}

Entity EntityFactory::createExplosion(uint8_t cellX, uint8_t cellY, Direction direction, uint8_t range, float spreadTime, BombType bombType)
{

	if (bombType == BombType::LIGHTNING && range == 0)
	{
		createExplosion(cellX, cellY, 1, spreadTime, BombType::LIGHTNING_PEAK);
		return Entity();
	}

	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;

	float width = float(GameConstants::CELL_WIDTH);
	float height = float(GameConstants::CELL_HEIGHT);

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
	default:
		assert(false);
	}

	switch (bombType)
	{
	case BombType::GHOST: entity.assign<ParticleComponent>(ParticleEffects::ghostBomb()); break;
	case BombType::LIGHTNING: entity.assign<ParticleComponent>(ParticleEffects::lightningBomb(range)); break;
	case BombType::LIGHTNING_PEAK: entity.assign<ParticleComponent>(ParticleEffects::lightningBombPeak()); break;
	default: entity.assign<ParticleComponent>(ParticleEffects::normalBomb()); break;
	}

	if (!entity.component<ParticleComponent>()->emitter)
		entity.remove<ParticleComponent>();

	entity.assign<SpreadComponent>(direction, range, spreadTime, bombType);
	entity.assign<ExplosionComponent>();
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<DestructionComponent>(0.55f);

	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(cellX, cellY);

	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);

	m_layerManager->add(entity);

	GameGlobals::events->emit<ExplosionCreatedEvent>(entity, cellX, cellY, direction, range, spreadTime, bombType);
	return entity;
}

Entity EntityFactory::createLava(uint8_t cellX, uint8_t cellY)
{
	Entity entity = GameGlobals::entities->create();

	auto floor = m_layerManager->getEntityWithComponent<FloorComponent>(GameConstants::FLOOR_LAYER, cellX, cellY);
	if (floor && !floor.has_component<DestructionComponent>())
		floor.assign<DestructionComponent>();

	TransformComponent transformComponent;

	float width = float(GameConstants::CELL_WIDTH);
	float height = float(GameConstants::CELL_HEIGHT);

	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f + 16.f;

	entity.assign<ParticleComponent>(ParticleEffects::lava());

	if (!entity.component<ParticleComponent>()->emitter)
		entity.remove<ParticleComponent>();

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::FLOOR_LAYER);
	entity.assign<LavaComponent>();
	entity.assign<NoNetComponent>();

	m_layerManager->add(entity);

	GameGlobals::events->emit(LavaCreatedEvent(cellX, cellY));
	return entity;
}

Entity EntityFactory::createItemSpawnEffect(uint8_t cellX, uint8_t cellY, entityx::Entity& item)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;

	float width = float(GameConstants::CELL_WIDTH);
	float height = float(GameConstants::CELL_HEIGHT);

	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f + 16.f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DestructionComponent>(1.5f);
	entity.assign<DynamicComponent>();
	entity.assign<ParticleComponent>(ParticleEffects::itemSpawn(item));

	if (!entity.component<ParticleComponent>()->emitter)
		entity.remove<ParticleComponent>();
	entity.assign<NoNetComponent>();

	m_layerManager->add(entity);

	return entity;
}

void EntityFactory::markLavaSpot(uint8_t cellX, uint8_t cellY)
{
	auto entity = m_layerManager->getEntityWithComponent<FloorComponent>(GameConstants::FLOOR_LAYER, cellX, cellY);
	if (entity && !entity.has_component<MarkedLavaSpotComponent>() && !entity.has_component<LavaComponent>())
	{
		entity.assign<LavaComponent>();
		entity.assign<MarkedLavaSpotComponent>();
		entity.assign<TimerComponent>(GameConstants::LAVA_SPAWN_TIME);
		GameGlobals::events->emit(LavaSpotMarkedEvent(cellX, cellY));
	}	
}

Entity EntityFactory::createFloor(uint8_t cellX, uint8_t cellY)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f + 7;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f + 5.f + 10;
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
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DestructionComponent>(3.f);
	entity.assign<EffectComponent>();

	auto emitter = ParticleEffects::smoke();

	if (emitter)
		entity.assign<ParticleComponent>(emitter);

	m_layerManager->add(entity);

	GameGlobals::events->emit<SmokeCreatedEvent>(entity, cellX, cellY);
	return entity;
}

Entity EntityFactory::createBoostEffect(uint8_t cellX, uint8_t cellY, Entity target)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH * 0.5f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT * 0.5f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<DestructionComponent>(1.5f);
	entity.assign<EffectComponent>();

	auto emitter = ParticleEffects::boostEffect(target);

	if (emitter)
		entity.assign<ParticleComponent>(emitter);

	m_layerManager->add(entity);

	GameGlobals::events->emit<BoostEffectCreatedEvent>(entity, cellX, cellY, target);
	return entity;
}

Entity EntityFactory::createItem(uint8_t cellX, uint8_t cellY, ItemType type)
{
	Entity entity = GameGlobals::entities->create();
	
	TransformComponent transformComponent;
	transformComponent.x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f + 7.f;
	transformComponent.y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f + 15.f;
	transformComponent.scaleX = 0.70f;
	transformComponent.scaleY = 0.70f;
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<CellComponent>(cellX, cellY);
	entity.assign<LayerComponent>(GameConstants::MAIN_LAYER);
	entity.assign<ItemComponent>(type);
	entity.assign<HealthComponent>(1);
	entity.assign<ExplosionStopComponent>();
	entity.assign<DynamicComponent>();

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
	case ItemType::BOMB_RANGE_BOOST:
		entity.assign<SpriteComponent>(createSprite("bombRangeBoost"));
		break;
	case ItemType::PORTAL_SKILL:
		entity.assign<SpriteComponent>(createSprite("portal_skill"));
		break;
	case ItemType::ANTI_MAGNET_SKILL:
		entity.assign<SpriteComponent>(createSprite("anti_magnet_skill"));
		break;
	case ItemType::PUNCH_SKILL:
		entity.assign<SpriteComponent>(createSprite("punch_skill"));
		break;
	case ItemType::HOLD_BOMB_SKILL:
		entity.assign<SpriteComponent>(createSprite("holding_skill"));
		break;
	case ItemType::BLINK_SKILL:
		entity.assign<SpriteComponent>(createSprite("blink_skill"));
		break;
	case ItemType::GHOST_BOMB:
		entity.assign<SpriteComponent>(createSprite("ghost_bomb_item"));
		break;
	case ItemType::LIGHTNING_BOMB:
		entity.assign<SpriteComponent>(createSprite("lightning_bomb_item"));
		break;
	default: break;
	}

	m_layerManager->add(entity);

	GameGlobals::events->emit<ItemCreatedEvent>(entity, cellX, cellY, type);
	return entity;
}

void EntityFactory::initAI(Entity& entity, uint8_t id)
{
	assert(entity && !entity.has_component<AIComponent>());
	assert(entity.has_component<InventoryComponent>() && entity.has_component<InputComponent>());

	entity.assign<AIComponent>(id);
	auto aiComponent = entity.component<AIComponent>();

	auto& personalities = GameGlobals::assetManager->getAIPersonalities();
	assert(personalities.size() > 0);

	if (id >= personalities.size())
	{
		cout << "EntityFactory AI-Init - WARNING: Not enough AI personalities. Taking first available." << endl;
		id = 0;
	}
	
	aiComponent->personality = personalities[id];
}

sf::Sprite EntityFactory::createSprite(const std::string& textureName)
{
	auto texture = GameGlobals::assetManager->getTexture(textureName);
	sf::Sprite sprite(*texture->get());
	sprite.setOrigin(texture->getRect().width*0.5f, texture->getRect().height*0.5f);
	sprite.setTextureRect(texture->getRect());

	return sprite;
}