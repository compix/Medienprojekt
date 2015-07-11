#pragma once
#include <string>

struct ConnectionStateEvent
{
	explicit ConnectionStateEvent(std::string message, bool finished = false) : message(message), finished(finished){}

	std::string message;
	bool finished;
};
