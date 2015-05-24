#pragma once
#include <string>
#include <entityx/Entity.h>

struct NetPlayerInfo
{
	std::string name = "Connecting...";
	bool connecting = true;
	entityx::Entity entity;
};
