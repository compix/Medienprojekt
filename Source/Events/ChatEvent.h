#pragma once
#include <string>

struct ChatEvent
{
	explicit ChatEvent(std::string message) : message(message) {}

	std::string message;
};
