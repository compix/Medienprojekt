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

EntityFactory::EntityFactory(EntityX* entityX, TextureLoader* textureLoader, PhysixSystem* physixSystem, LayerManager* layerManager)
	:m_entityX(entityX), m_textureLoader(textureLoader), m_PhysixSystem(physixSystem), m_layerManager(layerManager)
{
}

Entity EntityFactory::createTestEntity1(int row, int col)
{
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("char_idle");
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
	inputComponent.playerIndex = 0;
	entity.assign<InputComponent>(inputComponent);
	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createTestEntity2()
{
	sf::Sprite sprite;
	Texture& tex = m_textureLoader->get("char_death");
	sprite.setTexture(tex);

	Entity entity = m_entityX->entities.create();

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
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("block");
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


	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

entityx::Entity EntityFactory::createSolidBlock(int row, int col)
{
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("solid_block");
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

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createBomb(int row, int col)
{
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("bomb");
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

	entity.assign<CellComponent>(col, row);

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createExplosion(int row, int col, ExplosionDirection::Direction direction, int range, float spreadTime, float lifeTime, bool visible)
{
	Entity entity = m_entityX->entities.create();

	TransformComponent transformComponent;

	if (range == 0)
	{
		Texture& tex = m_textureLoader->get("exploEnd");
		sf::Sprite sprite;
		sprite.setTexture(tex);
		sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().y*0.5f);

		transformComponent.x = (float)tex.getSize().x * col + GameConstants::CELL_WIDTH*0.5f;
		transformComponent.y = (float)tex.getSize().y * row + GameConstants::CELL_HEIGHT*0.5f;

		switch (direction)
		{
		case ExplosionDirection::UP:
			transformComponent.rotation = -90.f;
			break;
		case ExplosionDirection::DOWN:
			transformComponent.rotation = 90.f;
			break;
		case ExplosionDirection::LEFT:
			transformComponent.rotation = 180.f;
			break;
		case ExplosionDirection::RIGHT:
			break;
		}

		if (visible)
			entity.assign<SpriteComponent>(sprite);
	}
	else
	{
		Texture& tex = m_textureLoader->get("subExplosion");
		sf::Sprite sprite;
		sprite.setTexture(tex);
		sprite.setOrigin(tex.getSize().x*0.5f, tex.getSize().y*0.5f);

		transformComponent.x = (float)tex.getSize().x * col + GameConstants::CELL_WIDTH*0.5f;
		transformComponent.y = (float)tex.getSize().y * row + GameConstants::CELL_HEIGHT*0.5f;

		if (direction == ExplosionDirection::UP || direction == ExplosionDirection::DOWN)
			transformComponent.rotation = 90.f;

		if (visible)
			entity.assign<SpriteComponent>(sprite);
	}

	entity.assign<ExplosionComponent>(direction, range, spreadTime);
	entity.assign<TransformComponent>(transformComponent);

	entity.assign<DamageDealerComponent>(1);
	entity.assign<CellComponent>(col, row);
	entity.assign<DestructionComponent>(lifeTime);

	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}

Entity EntityFactory::createExplosion(int row, int col, int range, float spreadTime)
{
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("explosion");
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

	float lifeTime = (range + 1)*spreadTime;
	createExplosion(row, col, ExplosionDirection::DOWN, range, spreadTime, lifeTime, false);
	createExplosion(row, col, ExplosionDirection::UP, range, spreadTime, lifeTime, false);
	createExplosion(row, col, ExplosionDirection::LEFT, range, spreadTime, lifeTime, false);
	createExplosion(row, col, ExplosionDirection::RIGHT, range, spreadTime, lifeTime, false);

	entity.assign<DestructionComponent>(lifeTime);
	entity.assign<LayerComponent>(0);

	m_layerManager->add(entity);

	return entity;
}