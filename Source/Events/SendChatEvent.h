#pragma once
#include <string>

struct SendChatEvent
{
	explicit SendChatEvent(std::string message) : message(message) {}

	std::string message;
};
