#include "EntityFactory.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/InputComponent.h"
#include "Components/BodyComponent.h"
#include <SFML/Graphics.hpp>


EntityFactory::EntityFactory(EntityX* entityX, TextureLoader* textureLoader, b2World* world, LayerManager* layerManager)
	:m_entityX(entityX), m_textureLoader(textureLoader), m_world(world), m_layerManager(layerManager)
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

// 	b2BodyDef bodyDef;
// 	bodyDef.type = b2_dynamicBody;
// 	bodyDef.position.Set(100.0f, 100.0f);
// 	b2Body* body = m_world->CreateBody(&bodyDef);
// 	b2PolygonShape dynamicBox;
// 	dynamicBox.SetAsBox(1.0f, 1.0f);
// 	b2FixtureDef fixtureDef;
// 	fixtureDef.shape = &dynamicBox;
// 	fixtureDef.density = 1.0f;
// 	fixtureDef.friction = 0.3f;
// 	body->CreateFixture(&fixtureDef);
// 	BodyComponent bodyComponent;
// 	bodyComponent.body = body;
// 
// 	entity.assign<BodyComponent>(bodyComponent);

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
	transformComponent.x = (float) tex.getSize().x * col;
	transformComponent.y = (float)tex.getSize().x * row;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	m_layerManager->addToLayer(0, entity);

	return entity;
}
