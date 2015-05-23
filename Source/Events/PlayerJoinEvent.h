#pragma once
#include <string>

struct PlayerJoinEvent
{
	explicit PlayerJoinEvent(std::string name) : name(name) {}

	std::string name;
};
