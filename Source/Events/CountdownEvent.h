#pragma once
#include <string>

struct CountdownEvent
{
	CountdownEvent(const std::string &message)
		: message(message) {}

	std::string message;
};