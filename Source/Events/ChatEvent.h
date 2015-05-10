#pragma once
#include <string>

struct ChatEvent
{
	explicit ChatEvent(std::string message, std::string name) : message(message), name(name) {}

	std::string message;
	std::string name;
};
