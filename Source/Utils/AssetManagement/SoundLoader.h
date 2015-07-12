#pragma once
#include <string>
#include <SFML/Audio.hpp>
#include "AssetLoader.h"

using sf::SoundBuffer;
using std::string;

class SoundLoader : public AssetLoader<SoundBuffer>
{
public:
	SoundLoader() : AssetLoader("Sounds", "Assets/sound/") {}

protected:
	bool preload(const string& key, const string &filename) override;
	bool load(const string& key, const Json::Value& jsonValue) override;

};

