#pragma once
#include  <string>

struct SoundComponent
{
	/**
	* @param playFrequency: How long to wait before the sound can be played again
	*/
	SoundComponent(float playFrequency, const std::string& soundName, float volume)
		: playFrequency(playFrequency), soundName(soundName), volume(volume) {}

	std::string soundName;
	float playFrequency; // In seconds
	float remaining; // In seconds
	float volume;
};