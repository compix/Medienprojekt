#pragma once
#include <string>

struct ReadyEvent
{
	explicit ReadyEvent(std::string name) : name(name) {}

	std::string name;
};
