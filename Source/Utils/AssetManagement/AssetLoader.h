#pragma once
#include <unordered_map>
#include <string>
#include "json/json.h"
#include <fstream>
#include <iostream>
#include "../Exceptions.h"

using std::unordered_map;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

struct PreloadFile
{
	std::string key;
	std::string filename;
};

class AssetLoaderBase
{
public:
	AssetLoaderBase(const std::string name, const std::string basePath) : m_name(name), m_basePath(basePath) {}
	virtual ~AssetLoaderBase() { }


	bool loadFromJson(const string& path);
	const std::string &getName() { return m_name; }
	int getPreloadCount() { return m_preloads.size(); }
	bool preloadsDone() { return m_preloads.empty(); }
	bool preloadNext();
	const std::string &getNextFilename();

protected:
	virtual bool preload(const string& key, const string &filename) = 0;
	virtual bool load(const string& key, const Json::Value& jsonValue) = 0;
	void addPreload(const string &key, const string &filename);

private:
	std::deque<PreloadFile> m_preloads;

protected:
	string m_name;
	string m_basePath;
};

template<class T>
class AssetLoader : public AssetLoaderBase
{
public:
	AssetLoader(const std::string name, const std::string basePath) : AssetLoaderBase(name, basePath) {}
	typedef unordered_map<string, T> AssetMap;

public:
	virtual T& get(const string& name);

protected:
	AssetMap m_assets;
};

template<class T>
T& AssetLoader<T>::get(const string& name)
{
	return m_assets.at(name);
}
