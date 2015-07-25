#pragma once
#include <string>
#include <vector>

enum class CreateGamePlayerType
{
	LOCAL,
	COMPUTER,
	CLIENT
};

struct CreateGamePlayerInfo
{
	explicit CreateGamePlayerInfo(std::string name, CreateGamePlayerType type) : name(name), type(type) {}
	std::string name;
	CreateGamePlayerType type;
};

struct CreateGameEvent
{
	explicit CreateGameEvent(uint8_t width, uint8_t height, std::vector<CreateGamePlayerInfo> players)
		: width(width), height(height), players(players), online(false) {}
	explicit CreateGameEvent(uint8_t width, uint8_t height, std::vector<CreateGamePlayerInfo> players, int port, int maxClients)
		: width(width), height(height), players(players), online(true), port(port), maxClients(maxClients) {}

	uint8_t width;
	uint8_t height;
	std::vector<CreateGamePlayerInfo> players;

	bool online;
	int port;
	int maxClients;
};
