#include "MusicLoader.h"

MusicLoader::MusicLoader()
{
	m_basePath = "Assets/music/";
}

MusicLoader::MusicLoader(std::string basePath)
{
	m_basePath = basePath;
}

Music& MusicLoader::load(const string& filename, const string& name)
{
	string pathToMusic;
	string path = m_basePath + filename;

	m_assets[name];

	auto& music = m_assets[name];
	if (!music.openFromFile(path))
		throw file_not_found(path);
	
	return m_assets[name];
}

Music& MusicLoader::load(const string& name, const Json::Value& jsonValue)
{
	return load(jsonValue.asString(), name);
}


bool MusicLoader::file_exists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
