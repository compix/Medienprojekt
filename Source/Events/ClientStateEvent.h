#pragma once
#include <string>

enum class ClientState
{
	CONNECTING,
	CONNECTED,
	LOBBY,
	PREGAME,
	DISCONNECTED
};

struct ClientStateEvent
{
	explicit ClientStateEvent(std::string message, ClientState state) : message(message), state(state){}

	std::string message;
	ClientState state;
};
