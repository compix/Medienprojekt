#pragma once
#include <string>
#include <SFML/Audio.hpp>
#include "AssetLoader.h"
#include <sys/stat.h>

using sf::SoundBuffer;
using std::string;

class MusicLoader : public AssetLoader<string>
{
public:
	MusicLoader();
	MusicLoader(std::string basePath);

	virtual string& load(const string& filename, const string& name) override;
	string& load(const string& name, const Json::Value& jsonValue) override;
	inline bool file_exists(const std::string& name);

};
