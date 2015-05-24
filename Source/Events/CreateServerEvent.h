#pragma once
#include <string>

struct CreateServerEvent
{
	explicit CreateServerEvent(std::string host, int port, int maxPlayers, std::string name) : host(host), port(port), maxPlayers(maxPlayers), name(name) {}

	std::string host;
	int port;
	int maxPlayers;
	std::string name;
};
