#pragma once
#include <entityx/entityx.h>
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
using entityx::EntityManager;

class EntityFactory
{
public:
	EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager);

	// TODO: Clean this up: remove code duplications. Need an easier way to create entities (with a builder).

	Entity createPlayer(float x, float y);

	Entity createBlock(uint8_t cellX, uint8_t cellY);
	Entity createSolidBlock(uint8_t cellX, uint8_t cellY);

	Entity createBomb(uint8_t cellX, uint8_t cellY, Entity owner);
	
	Entity createExplosion(uint8_t cellX, uint8_t cellY, Direction direction, uint8_t range, float spreadTime);
	Entity createExplosion(uint8_t cellX, uint8_t cellY, uint8_t range, float spreadTime);

	Entity createFloor(uint8_t cellX, uint8_t cellY);

	Entity createSmoke(uint8_t cellX, uint8_t cellY);

	Entity createBoostEffect(uint8_t cellX, uint8_t cellY, Entity target);

	Entity createItem(uint8_t cellX, uint8_t cellY, ItemType type);

	Entity* createEntity();
private:
	sf::Sprite createSprite(const std::string& textureName);
private:
	PhysixSystem* m_physixSystem;
	LayerManager* m_layerManager;
	ShaderManager* m_shaderManager;
	entityx::SystemManager* m_systemManager;
	unordered_map<std::uint64_t,Entity> m_entityMap;
};

