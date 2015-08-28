#pragma once
#include <string>
#include "../Utils/AssetManagement/AILoader.h"

using std::string;
struct SoundEvent
{
	explicit SoundEvent(const string &name) :SoundEvent(name, GameConstants::SOUND_VOLUME) {}
	SoundEvent(const string& name, float volume) : name(name), volume(volume) {}

	string name;
	float volume;
};

