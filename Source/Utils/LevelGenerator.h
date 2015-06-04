#pragma once
#include "../EntityFactory.h"

/*
The level is represented as a grid. The position is given by the cellX and cellY (indexing starts at 0).
The total number of cells in x axis (width) and cells in y axis (height) is odd.

The level is defined by rules:
- Indestructible blocks at the borders and if the cellX and cellY are even.
- Character (Player/Enemy) positions are predefined: Currently 4 characters at the corners.
- The horizontal and vertical neighbor tiles of the characters are empty.
- Destructible blocks at the remaining positions (spawned at a defined probability).

Note: The order of the rules might matter. The order defines the priority thus first rules are considered first.
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
	LevelGenerator(int width, int height);

	void generateRandomLevel();

private:
	bool characterRule(LevelPosition pos);
	bool emptyRule(LevelPosition pos);
	bool indestructibleBlockRule(LevelPosition pos);
	bool destructibleBlockRule(LevelPosition pos);

	bool neighbors(LevelPosition pos1, LevelPosition pos2);
private:
	std::vector<LevelPosition> m_characterPositions;

	int m_width;
	int m_height;
};

