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

EntityFactory::EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager)
	:m_PhysixSystem(physixSystem), m_layerManager(layerManager), m_shaderManager(shaderManager)
{
}

Entity EntityFactory::createTestEntity1(int row, int col)
{
	Entity entity = GameGlobals::entities->create();

	Texture& tex = GameGlobals::textures->get("char_idle");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = (float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)GameConstants::CELL_HEIGHT * row;
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
	bodyComponent.body = BodyFactory::CreateCircle((float)GameConstants::CELL_WIDTH * col + GameConstants::CELL_WIDTH*0.5f,
												(float)GameConstants::CELL_HEIGHT * row,
												10.f,
												b2_dynamicBody, 
												BodyFactory::CollsionCategory::PLAYER, 
												BodyFactory::CollsionCategory::SOLID_BLOCK);

	bodyComponent.body->SetFixedRotation(true);
	entity.assign<BodyComponent>(bodyComponent);

	InputComponent inputComponent;
	inputComponent.playerIndex = m_playerIndex++;
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
	entity.assign<BombComponent>(4, 0.06f);
	entity.assign<TimerComponent>(2.f);
	entity.assign<HealthComponent>(1);
	entity.assign<OwnerComponent>(owner);

	entity.assign<CellComponent>(col, row);

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createExplosion(int row, int col, Direction direction, int range, float spreadTime, bool visible)
{
	Entity entity = GameGlobals::entities->create();

	TransformComponent transformComponent;

	if (range == 0)
	{
		Texture& tex = GameGlobals::textures->get("exploEnd");
		sf::Sprite sprite;
		sprite.setTexture(tex);
		sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().y*0.5f);

		transformComponent.x = (float)tex.getSize().x * col + GameConstants::CELL_WIDTH*0.5f;
		transformComponent.y = (float)tex.getSize().y * row + GameConstants::CELL_HEIGHT*0.5f;

		switch (direction)
		{
		case Direction::UP:
			transformComponent.rotation = -90.f;
			break;
		case Direction::DOWN:
			transformComponent.rotation = 90.f;
			break;
		case Direction::LEFT:
			transformComponent.rotation = 180.f;
			break;
		case Direction::RIGHT:
			break;
		}

		if (visible)
		{
			entity.assign<LightComponent>(sf::Vector2f(transformComponent.x, transformComponent.y), sf::Color(255, 140, 0), 100.f, 360.f, 0.f);
			auto lightComponent = entity.component<LightComponent>();
			lightComponent->light.setShader(m_shaderManager->getLightShader());
			lightComponent->light.setAttenuation(sf::Vector3f(200.f, 10.f, 0.2f));
			entity.assign<SpriteComponent>(sprite);
		}
	}
	else
	{
		Texture& tex = GameGlobals::textures->get("subExplosion");
		sf::Sprite sprite;
		sprite.setTexture(tex);
		sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().y*0.5f);

		transformComponent.x = (float)tex.getSize().x * col + GameConstants::CELL_WIDTH*0.5f;
		transformComponent.y = (float)tex.getSize().y * row + GameConstants::CELL_HEIGHT*0.5f;

		if (direction == Direction::UP || direction == Direction::DOWN)
			transformComponent.rotation = 90.f;

		if (visible)
		{
			entity.assign<LightComponent>(sf::Vector2f(transformComponent.x, transformComponent.y), sf::Color(255, 140, 0), 100.f, 360.f, 0.f);
			auto lightComponent = entity.component<LightComponent>();
			lightComponent->light.setShader(m_shaderManager->getLightShader());
			lightComponent->light.setAttenuation(sf::Vector3f(200.f, 10.f, 0.2f));
			entity.assign<SpriteComponent>(sprite);
		}
			
	}

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

	Texture& tex = GameGlobals::textures->get("explosion");
	sf::Sprite sprite;
	sprite.setTexture(tex);
	sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().y*0.5f);
	tex.setRepeated(true);

	TransformComponent transformComponent;
	transformComponent.x = (float)tex.getSize().x * col + GameConstants::CELL_WIDTH*0.5f;
	transformComponent.y = (float)tex.getSize().y * row + GameConstants::CELL_HEIGHT*0.5f;

	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);
	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(col, row);
	
	LinkComponent linkComponent;
	linkComponent.links.push_back(createExplosion(row, col, Direction::DOWN, range, spreadTime, false));
	linkComponent.links.push_back(createExplosion(row, col, Direction::UP, range, spreadTime, false));
	linkComponent.links.push_back(createExplosion(row, col, Direction::LEFT, range, spreadTime, false));
	linkComponent.links.push_back(createExplosion(row, col, Direction::RIGHT, range, spreadTime, false));
	
	entity.assign<LightComponent>(sf::Vector2f(transformComponent.x, transformComponent.y), sf::Color(255, 140, 0), 200.f, 360.f, 0.f);
	auto lightComponent = entity.component<LightComponent>();
	lightComponent->light.setShader(m_shaderManager->getLightShader());

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