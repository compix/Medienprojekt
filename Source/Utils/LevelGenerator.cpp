#include "LevelGenerator.h"
#include <functional>
#include <vector>
#include "Random.h"

LevelGenerator::LevelGenerator(EntityFactory* entityFactory, int rowCount, int colCount)
	: m_entityFactory(entityFactory), m_rowCount(rowCount), m_colCount(colCount)
{
	m_characterPositions.push_back(LevelPosition(1, 1)); // top left corner
	m_characterPositions.push_back(LevelPosition(1, m_colCount - 2)); // top right corner
	m_characterPositions.push_back(LevelPosition(m_rowCount - 2, 1)); // bottom left corner
	m_characterPositions.push_back(LevelPosition(m_rowCount - 2, m_colCount - 2)); // bottom right corner
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

	for (int row = 0; row < m_rowCount; row++)
	{
		for (int col = 0; col < m_colCount; col++)
		{
			for (auto rule : rules)
				if ((this->*rule)(LevelPosition(row, col)))
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
		m_entityFactory->createTestEntity1(pos.row, pos.col);

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
	bool condition = pos.row == 0 || 
		 pos.row == m_rowCount - 1 || 
		 pos.col == 0 || 
		 pos.col == m_colCount - 1 || 
		 !((pos.row % 2) | (pos.col % 2)); // even

	if (condition)
		m_entityFactory->createSolidBlock(pos.row, pos.col);

	return condition;
}

bool LevelGenerator::destructibleBlockRule(LevelPosition pos)
{
	bool condition = Random::getInt(1, 100) <= 85; // 85% chance to spawn a block
	if (condition) 
		m_entityFactory->createBlock(pos.row, pos.col);

	return condition;
}

bool LevelGenerator::neighbors(LevelPosition pos1, LevelPosition pos2)
{
	return (pos1.row == pos2.row + 1) ||
		   (pos1.row == pos2.row - 1) || 
		   (pos1.col == pos2.col + 1) ||
		   (pos1.col == pos2.col - 1);
}


