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
	MusicLoader();
	MusicLoader(std::string basePath);

	virtual Music& load(const string& filename, const string& name) override;
	Music& load(const string& name, const Json::Value& jsonValue) override;
	inline bool file_exists(const std::string& name);

};
