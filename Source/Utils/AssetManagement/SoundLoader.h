#pragma once
#include <string>
#include <SFML/Audio.hpp>
#include "AssetLoader.h"

using sf::SoundBuffer;
using std::string;

class SoundLoader : public AssetLoader<SoundBuffer>
{
public:
	SoundLoader();
	SoundLoader(std::string basePath);

	virtual SoundBuffer& load(const string& filename, const string& name) override;
	SoundBuffer& load(const string& name, const Json::Value& jsonValue) override;

};

