#pragma once

struct MenuShowEvent
{
	explicit MenuShowEvent(bool visible) : visible(visible) {}

	bool visible;
};
