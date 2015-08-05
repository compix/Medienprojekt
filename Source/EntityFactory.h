#pragma once
#include<Box2D/Box2D.h>
#include "Utils/AssetManagement/TextureLoader.h"
#include "PhysixSystem.h"
#include "LayerManager.h"
#include "Utils/Common.h"
#include "Components/ExplosionComponent.h"
#include "Components/ItemComponent.h"

namespace sf{
	class SoundBuffer;
}

class ShaderManager;
class Engine;

class EntityFactory
{
public:
	EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager, Engine *engine);

	// TODO: Clean this up: remove code duplications. Need an easier way to create entities (with a builder).

	Entity *createPlayer(float x, float y, uint8_t playerIndex);

	Entity *createBlock(uint8_t cellX, uint8_t cellY);
	Entity *createSolidBlock(uint8_t cellX, uint8_t cellY);

	Entity *createBomb(uint8_t cellX, uint8_t cellY, Entity *owner);
	Entity *createPortal(uint8_t cellX, uint8_t cellY, Entity *owner);
	Entity *createExplosion(uint8_t cellX, uint8_t cellY, Direction direction, uint8_t range, float spreadTime);
	Entity *createExplosion(uint8_t cellX, uint8_t cellY, uint8_t range, float spreadTime);

	Entity *createFloor(uint8_t cellX, uint8_t cellY);

	Entity *createSmoke(uint8_t cellX, uint8_t cellY);

	Entity *createBoostEffect(uint8_t cellX, uint8_t cellY, Entity *target);

	Entity *createItem(uint8_t cellX, uint8_t cellY, ItemType type);
private:
	void setSprite(sf::Sprite &sprite, const std::string& textureName);
private:
	PhysixSystem* m_physixSystem;
	LayerManager* m_layerManager;
	ShaderManager* m_shaderManager;
	Engine *m_engine;
};

