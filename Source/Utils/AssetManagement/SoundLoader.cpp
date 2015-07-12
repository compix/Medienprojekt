#include "SoundLoader.h"

bool SoundLoader::preload(const string& key, const string& filename)
{
	return m_assets[key].loadFromFile(m_basePath + filename);
}

bool SoundLoader::load(const string& key, const Json::Value& jsonValue)
{
	addPreload(key, jsonValue.asString());
	return true;
}
