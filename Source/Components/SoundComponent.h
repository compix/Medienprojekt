#pragma once
#include <SFML/Audio.hpp>

struct SoundComponent
{
	SoundComponent(const char* file) :buffer(), sound()
	{
		if(!buffer.loadFromFile(file)){
			
		}

		sound.setBuffer(buffer);
	}

	sf::SoundBuffer buffer;
	sf::Sound sound;
};