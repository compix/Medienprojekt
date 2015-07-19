#pragma once

struct SetReadyEvent
{
	explicit SetReadyEvent(int playerIndex, bool ready) : playerIndex(playerIndex), ready(ready) {}

	int playerIndex;
	bool ready;
};
