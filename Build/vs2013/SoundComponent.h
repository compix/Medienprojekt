#pragma once
#include <SFML/Audio.hpp>
#include <iostream>

struct SoundComponent
{
	sf::SoundBuffer buffer;
	sf::Sound sound;
	
	SoundComponent(std::string& file) : buffer(), sound()
	{
		if (!buffer.loadFromFile(file))
			std::cout << "ERROR: Loading Sound;";
		sound.setBuffer(buffer);
	}
};