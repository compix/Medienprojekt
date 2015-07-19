#pragma once
#include "../GameConstants.h"

struct LobbyEvent
{
	LobbyEvent(int numPlayers)
		: numPlayers(numPlayers)
	{
		for (int i = 0; i < numPlayers; i++)
		{
			enabled[i] = false;
			ready[i] = false;
		}
	}

	int numPlayers;
	std::string name[GameConstants::MAX_PLAYERS];
	bool enabled[GameConstants::MAX_PLAYERS];
	bool ready[GameConstants::MAX_PLAYERS];
};
