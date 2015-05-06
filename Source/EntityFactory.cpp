#include "EntityFactory.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/InputComponent.h"
#include "Components/BodyComponent.h"
#include <SFML/Graphics.hpp>
#include "BodyFactory.h"


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
	transformComponent.x = 25.f * col + 12.f;
	transformComponent.y = 25.f * row;
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

	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox(25.f * col + 12.f, 
												25.f * row + 15.f,
												10.f, 
												10.f, 
												b2_dynamicBody, 
												BodyFactory::CollsionCategory::PLAYER, 
												BodyFactory::CollsionCategory::SOLID_BLOCK);
	bodyComponent.body->SetFixedRotation(true);
	entity.assign<BodyComponent>(bodyComponent);

	InputComponent inputComponent;
	inputComponent.playerIndex = 0;
	entity.assign<InputComponent>(inputComponent);

	m_layerManager->addToLayer(0, entity);

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

	m_layerManager->addToLayer(0, entity);

	return entity;
}

entityx::Entity EntityFactory::createBlock(int row, int col)
{
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("block");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = (float) tex.getSize().x * col;
	transformComponent.y = (float) tex.getSize().x * row;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	m_layerManager->addToLayer(0, entity);

	return entity;
}

entityx::Entity EntityFactory::createSolidBlock(int row, int col)
{
	Entity entity = m_entityX->entities.create();

	Texture& tex = m_textureLoader->get("solid_block");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = (float)tex.getSize().x * col;
	transformComponent.y = (float)tex.getSize().x * row;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	BodyComponent bodyComponent;
	bodyComponent.body = BodyFactory::CreateBox(tex.getSize().x * col + sprite.getLocalBounds().width/2.f,
												(tex.getSize().x * row + sprite.getLocalBounds().height/2.f),
												tex.getSize().x / 2,
												tex.getSize().x / 2, 
												b2_staticBody, 
												BodyFactory::CollsionCategory::SOLID_BLOCK,
												BodyFactory::CollsionCategory::PLAYER);

	entity.assign<BodyComponent>(bodyComponent);

	m_layerManager->addToLayer(0, entity);

	return entity;
}
