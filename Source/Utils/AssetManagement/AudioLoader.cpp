#include "AudioLoader.h"


AudioLoader::AudioLoader()
{
	m_basePath = "Assets/music/";
}

AudioLoader::AudioLoader(std::string basePath)
{
	m_basePath = basePath;
}

SoundBuffer& AudioLoader::load(const string& filename, const string& name)
{
	SoundBuffer buffer;
	string path = m_basePath + filename;
	if (!buffer.loadFromFile(path))
		throw file_not_found(path);

	m_assets[name] = buffer;
	return m_assets[name];
}

SoundBuffer& AudioLoader::load(const string& name, const Json::Value& jsonValue)
{
	return load(jsonValue.asString(), name);
}
