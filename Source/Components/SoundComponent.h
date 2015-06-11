#pragma once
#include <SFML/Audio.hpp>

struct SoundComponent
{
	SoundComponent(const char* file, bool destroyEntityAtEnd = false) :buffer(), sound(), destroyEntityAtEnd(destroyEntityAtEnd)
	{
		if(!buffer.loadFromFile(file)){
			std::cout << "SOUND_ERROR" << std::endl;
		}

		sound.setBuffer(buffer);
	}

	sf::SoundBuffer buffer;
	sf::Sound sound;
	bool wasPlayedOnce = false, destroyEntityAtEnd = false;
};