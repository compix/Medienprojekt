#include "MusicLoader.h"

MusicLoader::MusicLoader()
{
	m_basePath = "Assets/audio/";
}

MusicLoader::MusicLoader(std::string basePath)
{
	m_basePath = basePath;
}

string& MusicLoader::load(const string& filename, const string& name)
{
	string pathToMusic;
	string path = m_basePath + filename;

	
	if (!file_exists(path))
		throw file_not_found(path);

	m_assets[name] = path;
	return m_assets[name];
}

string& MusicLoader::load(const string& name, const Json::Value& jsonValue)
{
	return load(jsonValue.asString(), name);
}


bool MusicLoader::file_exists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}
