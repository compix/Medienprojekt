#pragma once
#include <string>

struct ForceDisconnectEvent
{
	ForceDisconnectEvent(const std::string &message="")
		: message(message) {}

	std::string message;
};
