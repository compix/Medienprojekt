#pragma once
#include <SFML/Audio.hpp>

struct SoundComponent
{
	SoundComponent(sf::SoundBuffer* buffer, bool destroyEntityAtEnd = false) :sound(), destroyEntityAtEnd(destroyEntityAtEnd)
	{
		sound.setBuffer(*buffer);
	}
	sf::Sound sound;
	bool wasPlayedOnce = false, destroyEntityAtEnd = false;
};