#include "TextureLoader.h"

bool TextureLoader::preload(const string& key, const string& filename)
{
	return m_assets[key].loadFromFile(m_basePath + filename);
}

bool TextureLoader::load(const string& key, const Json::Value& jsonValue)
{
	addPreload(key, jsonValue.asString());
	return true;
}