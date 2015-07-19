#pragma once
#include <string>

struct CountdownEvent
{
	CountdownEvent(std::string message)
		: message(message) {}

	std::string message;
};