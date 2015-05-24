#pragma once
#include <string>

struct DisconnectEvent
{
	explicit DisconnectEvent(std::string reason) : reason(reason) {}

	std::string reason;
};
