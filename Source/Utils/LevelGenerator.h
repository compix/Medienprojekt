#pragma once
#include <entityx/entityx.h>
#include <vector>
#include "../Components/ItemComponent.h"

struct ItemSpawnProperty
{
	ItemSpawnProperty(ItemType itemType, uint16_t min, uint16_t max) 
		: itemType(itemType), min(min), max(max) {}

	uint16_t min;
	uint16_t max;
	ItemType itemType;
};

struct LevelConfig
{
	LevelConfig() {}
	LevelConfig(uint8_t width, uint8_t height) : width(width), height(height) {}

	LevelConfig& itemSpawnProperty(ItemType type, uint16_t min, uint16_t max)
	{
		itemSpawnProperties.push_back(ItemSpawnProperty(type, min, max));
		return *this;
	}

	float blockDensity; // In [0, 1] where 0 = No blocks and 1 = Max number of blocks
	uint8_t width;
	uint8_t height;

	std::vector<ItemSpawnProperty> itemSpawnProperties;
};

struct DefaultConfig : LevelConfig
{
	DefaultConfig(uint8_t width, uint8_t height) : LevelConfig(width, height)
	{
		uint16_t numIndestructibleBlocks = (width - 3) * (height - 3) / 4 + (height + width - 2) * 2;
		uint16_t numFreeBlocks = 3 * 4; // At the 4 player corners
		uint16_t maxBlockCount = width * height - numIndestructibleBlocks - numFreeBlocks;
		blockDensity = 0.85f;
		float uniformChance = blockDensity * 0.125f;
		uint16_t itemCount = uint16_t(uniformChance * maxBlockCount);
		itemSpawnProperty(ItemType::BOMB_CAP_BOOST,		 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::ANTI_MAGNET_SKILL,	 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::BOMB_KICK_SKILL,	 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::BOMB_RANGE_BOOST,	 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::HOLD_BOMB_SKILL,	 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::PORTAL_SKILL,		 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::PUNCH_SKILL,		 uint16_t(itemCount), uint16_t(itemCount));
		itemSpawnProperty(ItemType::SPEED_MULTIPLICATOR, uint16_t(itemCount), uint16_t(itemCount));
	}
};

/**
* The level is represented as a grid. The position is given by the cellX and cellY (indexing starts at 0).
* The total number of cells in x axis (width) and cells in y axis (height) is odd.
* 
* The level is defined by rules:
* - Indestructible blocks at the borders and if the cellX and cellY are even.
* - Character (Player/Enemy) positions are predefined: Currently 4 characters at the corners.
* - The horizontal and vertical neighbor tiles of the characters are empty.
* - Destructible blocks at the remaining positions (spawned at a defined probability).
* 
* Note: The order of the rules matters. Only the first rule that meets the condition applies.
*/
class LevelGenerator
{
	struct LevelPosition
	{
		LevelPosition(int cellX, int cellY) : cellX(cellX), cellY(cellY) {}
		LevelPosition() {};

		bool operator==(const LevelPosition& other) { return cellX == other.cellX && cellY == other.cellY; }
		int cellX;
		int cellY;
	};

	typedef bool (LevelGenerator::*Rule)(LevelPosition); // Rules have the position in question as input and return true if the condition applies.
public:
	LevelGenerator(LevelConfig levelConfig);

	void generateRandomLevel();

private:
	bool characterRule(LevelPosition pos);
	bool emptyRule(LevelPosition pos);
	bool indestructibleBlockRule(LevelPosition pos);
	bool destructibleBlockRule(LevelPosition pos);

	bool neighbors(LevelPosition pos1, LevelPosition pos2);

	void spawnItems();

private:
	std::vector<LevelPosition> m_characterPositions;
	std::vector<entityx::Entity> m_destructibleBlocks;

	int m_playerIndex = 0;
	uint8_t m_width;
	uint8_t m_height;

	LevelConfig m_levelConfig;
};

