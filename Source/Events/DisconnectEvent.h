#pragma once
#include <string>

struct NetPlayerInfo;

struct DisconnectEvent
{
	explicit DisconnectEvent(std::string reason, NetPlayerInfo *playerInfo) : reason(reason), playerInfo(playerInfo) {}

	std::string reason;
	NetPlayerInfo *playerInfo;
};
