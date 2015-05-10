#pragma once
#include <string>

struct JoinServerEvent
{
	explicit JoinServerEvent(std::string host, int port, std::string name) : host(host), port(port), name(name) {}

	std::string host;
	int port;
	std::string name;
};
