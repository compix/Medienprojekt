#pragma once

struct ReadyEvent
{
	explicit ReadyEvent(uint8_t playerIndex, bool ready) : playerIndex(playerIndex), ready(ready) {}

	uint8_t playerIndex;
	bool ready;
};
