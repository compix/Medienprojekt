#pragma once
#include <string>
#include <vector>

struct CreateLocalGameEvent
{
	explicit CreateLocalGameEvent(std::vector<std::string> names) : names(names) {}

	std::vector<std::string> names;
};
