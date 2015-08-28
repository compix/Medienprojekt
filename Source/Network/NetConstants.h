#pragma once
#include <stdint.h>

enum class MessageType : uint16_t
{
	HANDSHAKE,
	CHAT,
	PLAYER_JOINED,
	PLAYER_READY,
	CREATE_BLOCK,
	CREATE_SOLID_BLOCK,
	CREATE_FLOOR,
	CREATE_PLAYER,
	CREATE_BOMB,
	CREATE_EXPLOSION,
	CREATE_PORTAL,
	CREATE_ITEM,
	CREATE_BOOST_EFFECT,
	CREATE_SMOKE,
	DEATH,
	DESTROY_ENTITY,
	UPDATE_DYNAMIC,
	SKILL,
	HOLDING_STATUS,
	START_GAME,
	PHASE2_STARTED,
	MARK_LAVA_SPOT,
	CREATE_LAVA,
	COUNTDOWN,
	ALL_READY,
	GAME_OVER,
	RESET_GAME,

	INPUT_DIRECTION,
	INPUT_BOMB_ACTIVATED,
	INPUT_SKILL_ACTIVATED,

	COUNT
};

enum class NetChannel : uint8_t
{
	CHAT,
	WORLD_RELIABLE,
	WORLD_UNRELIABLE,
	INPUT_RELIABLE,
	INPUT_UNRELIABLE,

	COUNT
};

enum class ServerStatus : uint8_t
{
	INIT,
	LOBBY,
	LOBBY_COUNTDOWN,
	INGAME,
	SHUTDOWN
};

namespace NetConstants
{
	const int MAX_CLIENTS = 32;
	const int DEFAULT_PORT = 1234;
}
