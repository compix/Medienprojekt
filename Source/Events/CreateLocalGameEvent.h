#pragma once
#include <string>
#include <vector>

struct CreateLocalGameEvent
{
	explicit CreateLocalGameEvent(uint8_t width, uint8_t height, std::vector<std::string> names) : width(width), height(height), names(names) {}

	uint8_t width;
	uint8_t height;
	std::vector<std::string> names;
};
