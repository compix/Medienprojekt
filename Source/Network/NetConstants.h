#pragma once
#include <stdint.h>

enum class MessageType : uint16_t
{
	HANDSHAKE,
	CHAT,
	PLAYER_JOINED,
	CREATE_BLOCK,
	CREATE_SOLID_BLOCK,
	CREATE_FLOOR,
	CREATE_PLAYER,
	CREATE_BOMB,
	CREATE_EXPLOSION,
	DESTROY_ENTITY,
	START_GAME,

	COUNT
};

enum class NetChannel : uint8_t
{
	HANDSHAKE,
	CHAT,
	STATUS,
	WORLD,
	MOVEMENT,

	COUNT
};

namespace NetConstants
{
	const int MAX_CLIENTS = 32;
	const int DEFAULT_PORT = 1234;
}
