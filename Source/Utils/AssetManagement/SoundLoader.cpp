#include "SoundLoader.h"


SoundLoader::SoundLoader()
{
	m_basePath = "Assets/sound/";
}

SoundLoader::SoundLoader(std::string basePath)
{
	m_basePath = basePath;
}

SoundBuffer& SoundLoader::load(const string& filename, const string& name)
{
	SoundBuffer buffer;
	string path = m_basePath + filename;
	if (!buffer.loadFromFile(path))
			throw file_not_found(path);


	m_assets[name] = buffer;
	return m_assets[name];
}

SoundBuffer& SoundLoader::load(const string& name, const Json::Value& jsonValue)
{
	return load(jsonValue.asString(), name);
}
