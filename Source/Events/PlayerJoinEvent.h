#pragma once
#include <string>

struct PlayerJoinEvent
{
	explicit PlayerJoinEvent(uint8_t playerIndex, std::string name) : playerIndex(playerIndex), name(name) {}

	uint8_t playerIndex;
	std::string name;
};
