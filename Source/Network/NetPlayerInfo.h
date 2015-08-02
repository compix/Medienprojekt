#pragma once
#include <string>
#include "ecstasy/core/Entity.h"


enum class NetPlayerStatus
{
	DISCONNECTED,
	CONNECTING,
	CONNECTED,
	READY
};

enum class CreateGamePlayerType
{
	LOCAL,
	COMPUTER,
	CLIENT
};

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

struct NetPlayerInfo
{
	std::string name = "Connecting...";
	NetPlayerStatus status = NetPlayerStatus::DISCONNECTED;
	Entity *entity;
	uint8_t playerIndex;
	CreateGamePlayerType type;
	ENetPeer *peer = nullptr;

	void init(ENetPeer *_peer)
	{
		peer = _peer;
		name = "Connecting...";
		status = NetPlayerStatus::CONNECTING;
		entity = nullptr;
	}

	void invalidate()
	{
		peer = nullptr;
		name = "?";
		status = NetPlayerStatus::DISCONNECTED;
		entity = nullptr;
	}
};
