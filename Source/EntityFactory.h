#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/AssetManagement/TextureLoader.h"
#include "PhysixSystem.h"
#include "LayerManager.h"
#include "Utils/Common.h"
#include "Components/ExplosionComponent.h"
#include "Components/ItemComponent.h"

enum class BombType : uint8_t;

namespace sf{
	class SoundBuffer;
}

class ShaderManager;
using entityx::EntityManager;

class EntityFactory
{
public:
	EntityFactory(bool isClient, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager);

	Entity createPlayer(float x, float y, uint8_t playerIndex);

	Entity createBlock(uint8_t cellX, uint8_t cellY);
	Entity createSolidBlock(uint8_t cellX, uint8_t cellY);

	/**
	* ghost = won't be stopped by anything except solid blocks
	* lightning = 1 range explosion when it is stopped
	*/
	Entity createBomb(uint8_t cellX, uint8_t cellY, Entity owner, BombType type);
	Entity createPortal(uint8_t cellX, uint8_t cellY, Entity owner, bool linked);
	Entity createAfterimage(int cellX, int cellY, float posX, float posY, sf::Sprite sprite, float time);
	/**
	* ghost = won't be stopped by anything except solid blocks
	* lightning = 1 range explosion when it is stopped
	*/
	void createExplosion(uint8_t cellX, uint8_t cellY, uint8_t range, float spreadTime, BombType bombType);
	Entity createExplosion(uint8_t cellX, uint8_t cellY, Direction direction, uint8_t range, float spreadTime, BombType bombType);

	Entity createFloor(uint8_t cellX, uint8_t cellY);

	Entity createSmoke(uint8_t cellX, uint8_t cellY);

	Entity createBoostEffect(uint8_t cellX, uint8_t cellY, Entity target);

	Entity createItem(uint8_t cellX, uint8_t cellY, ItemType type);

	void initAI(Entity& entity, uint8_t id);
private:
	sf::Sprite createSprite(const std::string& textureName);
private:
	bool m_isClient;
	LayerManager* m_layerManager;
	ShaderManager* m_shaderManager;
	entityx::SystemManager* m_systemManager;
};

