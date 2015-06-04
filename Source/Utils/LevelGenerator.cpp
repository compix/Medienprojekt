#include "LevelGenerator.h"
#include <functional>
#include <vector>
#include "Random.h"
#include "../GameGlobals.h"

LevelGenerator::LevelGenerator(int width, int height)
	: m_width(width), m_height(height)
{
	m_characterPositions.push_back(LevelPosition(1, 1)); // top left corner
	m_characterPositions.push_back(LevelPosition(width - 2, 1)); // top right corner
	m_characterPositions.push_back(LevelPosition(1, height - 2)); // bottom left corner
	m_characterPositions.push_back(LevelPosition(width - 2, height - 2)); // bottom right corner
}

void LevelGenerator::generateRandomLevel()
{
	Random::randomize();

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
			GameGlobals::entityFactory->createFloor(cellX, cellY);
			for (auto rule : rules)
				if ((this->*rule)(LevelPosition(cellX, cellY)))
					break;
		}
	}
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
		GameGlobals::entityFactory->createPlayer(x, y);
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
	bool condition = Random::getInt(1, 100) <= 85; // 85% chance to spawn a block
	if (condition) 
		GameGlobals::entityFactory->createBlock(pos.cellX, pos.cellY);

	return condition;
}

bool LevelGenerator::neighbors(LevelPosition pos1, LevelPosition pos2)
{
	return ((pos1.cellX == pos2.cellX) && ((pos1.cellY == pos2.cellY + 1) || (pos1.cellY == pos2.cellY - 1))) ||
		   ((pos1.cellY == pos2.cellY) && ((pos1.cellX == pos2.cellX + 1) || (pos1.cellX == pos2.cellX - 1)));
}


