#pragma once
#include "../EntityFactory.h"

/*
The level is represented as a matrix. The position is given by the row and column (indexing starts at 0).
The total number of rows and columns is odd.

The level is defined by rules:
- Indestructible blocks at the borders and if the row and column are even.
- Character (Player/Enemy) positions are predefined: Currently 4 characters at the corners.
- The horizontal and vertical neighbor tiles of the characters are empty.
- Destructible blocks at the remaining positions (spawned at a defined probability).

Note: The order of the rules might matter. The order defines the priority thus first rules are considered first.
*/
class LevelGenerator
{
	struct LevelPosition
	{
		LevelPosition(int row, int col) : row(row), col(col) {}
		LevelPosition() {};

		bool operator==(const LevelPosition& other) { return row == other.row && col == other.col; }
		int row;
		int col;
	};

	typedef bool (LevelGenerator::*Rule)(LevelPosition); // Rules have the position in question as input and return true if the condition applies.
public:
	LevelGenerator(EntityFactory* entityFactory, int rowCount, int colCount);

	void generateRandomLevel();

private:
	bool characterRule(LevelPosition pos);
	bool emptyRule(LevelPosition pos);
	bool indestructibleBlockRule(LevelPosition pos);
	bool destructibleBlockRule(LevelPosition pos);

	bool neighbors(LevelPosition pos1, LevelPosition pos2);
private:
	EntityFactory* m_entityFactory;

	std::vector<LevelPosition> m_characterPositions;

	int m_rowCount;
	int m_colCount;
};

