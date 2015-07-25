#include "MusicLoader.h"

MusicLoader::~MusicLoader()
{
	for (auto& m : m_assets)
		m.second.stop();
}


bool MusicLoader::preload(const string& key, const string &filename)
{
	return m_assets[key].openFromFile(m_basePath + filename);
}

bool MusicLoader::load(const string& key, const Json::Value& jsonValue)
{
	addPreload(key, jsonValue.asString());
	return true;
}
