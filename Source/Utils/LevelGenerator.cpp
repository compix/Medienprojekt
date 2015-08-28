#include "LevelGenerator.h"
#include "../EntityFactory.h"
#include <vector>
#include "Random.h"
#include "../GameGlobals.h"
#include "../Components/ItemSpawnerComponent.h"
#include <queue>

LevelGenerator::LevelGenerator(LevelConfig levelConfig)
	:m_levelConfig(levelConfig), m_width(levelConfig.width), m_height(levelConfig.height)
{
	m_characterPositions.push_back(LevelPosition(1, 1)); // top left corner
	m_characterPositions.push_back(LevelPosition(m_width - 2, 1)); // top right corner
	m_characterPositions.push_back(LevelPosition(1, m_height - 2)); // bottom left corner
	m_characterPositions.push_back(LevelPosition(m_width - 2, m_height - 2)); // bottom right corner
}

void LevelGenerator::generateRandomLevel()
{
	m_destructibleBlocks.clear();

	// Rules because lots of if then else statements are ugly, hard to read and harder to change.
	std::vector<Rule> rules;
	rules.push_back(&LevelGenerator::indestructibleBlockRule);
	rules.push_back(&LevelGenerator::characterRule);
	rules.push_back(&LevelGenerator::emptyRule);
	rules.push_back(&LevelGenerator::destructibleBlockRule);

	for (int cellY = 0; cellY < m_height; cellY++)
	{
		for (int cellX = 0; cellX < m_width; cellX++)
		{
			// No rules for the floor layer. Just floor.
			GameGlobals::entityFactory->createFloor(cellX, cellY);

			for (auto rule : rules)
				if ((this->*rule)(LevelPosition(cellX, cellY)))
					break;
		}
	}

	spawnItems();
}

bool LevelGenerator::characterRule(LevelPosition pos)
{
	bool condition = false;

	for (unsigned int i = 0; i < m_characterPositions.size() && !condition; i++)
		condition = pos == m_characterPositions[i];

	if (condition)
	{
		float x = (float)GameConstants::CELL_WIDTH * pos.cellX + GameConstants::CELL_WIDTH*0.5f;
		float y = (float)GameConstants::CELL_HEIGHT * pos.cellY;
		GameGlobals::entityFactory->createPlayer(x, y, m_playerIndex++);
	}

	return condition;
}

bool LevelGenerator::emptyRule(LevelPosition pos)
{
	bool condition = false;

	for (unsigned int i = 0; i < m_characterPositions.size() && !condition; i++)
		condition = neighbors(pos, m_characterPositions[i]);

	return condition;
}

bool LevelGenerator::indestructibleBlockRule(LevelPosition pos)
{
	bool condition = pos.cellY == 0 || 
		 pos.cellY == m_height - 1 || 
		 pos.cellX == 0 || 
		 pos.cellX == m_width - 1 ||
		 !((pos.cellY % 2) | (pos.cellX % 2)); // even

	if (condition)
		GameGlobals::entityFactory->createSolidBlock(pos.cellX, pos.cellY);

	return condition;
}

bool LevelGenerator::destructibleBlockRule(LevelPosition pos)
{
	bool condition = Random::getFloat(0, 1) <= m_levelConfig.blockDensity;
	if (condition) 
		m_destructibleBlocks.push_back(GameGlobals::entityFactory->createBlock(pos.cellX, pos.cellY));

	return condition;
}

bool LevelGenerator::neighbors(LevelPosition pos1, LevelPosition pos2)
{
	return ((pos1.cellX == pos2.cellX) && ((pos1.cellY == pos2.cellY + 1) || (pos1.cellY == pos2.cellY - 1))) ||
		   ((pos1.cellY == pos2.cellY) && ((pos1.cellX == pos2.cellX + 1) || (pos1.cellX == pos2.cellX - 1)));
}

void LevelGenerator::spawnItems()
{
	// If too many items were specified then some of the rare items might not spawn at all
	// Solution: Alternate item types in the vector
	std::unordered_map<ItemType, int, EnumClassHash> itemCounts;
	for (auto& itemSpawnProperty : m_levelConfig.itemSpawnProperties)
		itemCounts[itemSpawnProperty.itemType] = Random::getInt(itemSpawnProperty.min, itemSpawnProperty.max);

	std::queue<ItemType> itemTypes;
	bool allEmpty = false;
	while (!allEmpty)
	{
		allEmpty = true;
		for (auto& pair : itemCounts)
		{
			int& num = pair.second;
			if (num > 0)
			{
				allEmpty = false;
				--num;
				itemTypes.push(pair.first);
			}
		}
	}

	std::shuffle(m_destructibleBlocks.begin(), m_destructibleBlocks.end(), Random::RNG());
	uint16_t itemNum = 0;

	for (auto& block : m_destructibleBlocks)
	{
		if (itemTypes.empty())
			break;

		assert(!block.has_component<ItemSpawnerComponent>());
		block.assign<ItemSpawnerComponent>(itemTypes.front());
		itemTypes.pop();
	}

	if (!itemTypes.empty())
		std::cout << "LevelGenerator: Not all items could be spawned. This might be not intended." << std::endl;
}

