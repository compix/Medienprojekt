#pragma once
#include <string>

struct CreateServerEvent
{
	explicit CreateServerEvent(uint8_t width, uint8_t height, std::string host, int port, int maxPlayers, std::string name) : width(width), height(height), host(host), port(port), maxPlayers(maxPlayers), name(name) {}

	uint8_t width;
	uint8_t height;
	std::string host;
	int port;
	int maxPlayers;
	std::string name;
};
