#pragma once
#include  <string>

struct SoundComponent
{
	/**
	* @param playFrequency: How long to wait before the sound can be played again
	*/
	SoundComponent(float playFrequency, const std::string& soundName, float volume)
		: playFrequency(playFrequency), soundName(soundName), volume(volume) {}


	float playFrequency; // In seconds
    std::string soundName;
    float volume;
	float remaining; // In seconds
};
