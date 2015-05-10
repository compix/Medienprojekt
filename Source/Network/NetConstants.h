#pragma once
#include <stdint.h>

enum class MessageType : uint16_t
{
	CHAT,

	COUNT
};

enum class NetChannel : uint8_t
{
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
