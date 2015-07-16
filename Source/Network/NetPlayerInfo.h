#pragma once
#include <string>
#include <entityx/Entity.h>
#include "../Events/CreateGameEvent.h"

enum class NetPlayerStatus
{
	DISCONNECTED,
	CONNECTING,
	CONNECTED,
	READY
};

struct NetPlayerInfo
{
	std::string name = "Connecting...";
	NetPlayerStatus status = NetPlayerStatus::DISCONNECTED;
	entityx::Entity entity;
	uint8_t playerIndex;
	CreateGamePlayerType type;
	ENetPeer *peer = nullptr;

	void init(ENetPeer *_peer)
	{
		peer = _peer;
		name = "Connecting...";
		status = NetPlayerStatus::CONNECTING;
		entity.invalidate();
	}

	void invalidate()
	{
		peer = nullptr;
		name = "?";
		status = NetPlayerStatus::DISCONNECTED;
		entity.invalidate();
	}
};
