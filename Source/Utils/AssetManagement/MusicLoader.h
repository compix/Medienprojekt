#pragma once
#include <string>
#include <SFML/Audio.hpp>
#include "AssetLoader.h"
#include <sys/stat.h>

using sf::Music;
using std::string;

class MusicLoader : public AssetLoader<Music>
{
public:
	MusicLoader() : AssetLoader("Music", "Assets/music/") {}
	~MusicLoader();

protected:
	bool preload(const string& key, const string &filename) override;
	bool load(const string& key, const Json::Value& jsonValue) override;
};
