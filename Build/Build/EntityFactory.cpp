#include "EntityFactory.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "AnimationComponent.h"
#include <SFML/Graphics.hpp>

EntityFactory::EntityFactory(EntityX* pEntityX, TextureLoader* pTextureLoader)
	:m_pEntityX(pEntityX), m_pTextureLoader(pTextureLoader)
{

}

Entity EntityFactory::createTestEntity1()
{
	Entity entity = m_pEntityX->entities.create();

	Texture& tex = m_pTextureLoader->get("char_idle");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = 100.f;
	transformComponent.y = 100.f;
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

	return entity;
}

Entity EntityFactory::createTestEntity2()
{
	sf::Sprite sprite;
	Texture& tex = m_pTextureLoader->get("char_death");
	sprite.setTexture(tex);

	Entity entity = m_pEntityX->entities.create();

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

	return entity;
}

entityx::Entity EntityFactory::createBlock(float x, float y)
{
	Entity entity = m_pEntityX->entities.create();

	Texture& tex = m_pTextureLoader->get("block");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = x;
	transformComponent.y = y;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	return entity;
}

entityx::Entity EntityFactory::createSolidBlock(float x, float y)
{
	Entity entity = m_pEntityX->entities.create();

	Texture& tex = m_pTextureLoader->get("solid_block");
	sf::Sprite sprite;
	sprite.setTexture(tex);

	TransformComponent transformComponent;
	transformComponent.x = x;
	transformComponent.y = y;
	entity.assign<TransformComponent>(transformComponent);
	entity.assign<SpriteComponent>(sprite);

	return entity;
}
