#pragma once
#include <unordered_map>
#include <string>
#include <SFML/Graphics.hpp>
#include <memory>
#include "json/json.h"
#include <fstream>
#include <iostream>

using namespace sf;
using namespace std;

template<class T>
class AssetLoader
{
	typedef unordered_map<string, T> AssetMap;
public:
	virtual ~AssetLoader() {};

	virtual bool loadAllFromJson(const string& path);
	virtual const T& load(const string& filename);
	virtual const T& load(const string& filename, const string& name) = 0;
	virtual const T& get(const string& name) const;

	void setBasePath(const string& path);
protected:
	AssetMap m_assets;
	string m_basePath;
};

template<class T>
bool AssetLoader<T>::loadAllFromJson(const string& path)
{
	Json::Value root;
	Json::Reader reader;
	ifstream ifs(path, ifstream::binary);

	if (!reader.parse(ifs, root, false))
	{
		cout << "Parsing error: " << reader.getFormattedErrorMessages() << endl;
		return false;
	}

	for (auto it = root.begin(); it != root.end(); it++)
	{
		load(it->asString(), it.key().asString());
	}

	return true;
}

template<class T>
const T&  AssetLoader<T>::get(const string& name) const
{
	return m_assets.at(name);
}

template<class T>
const T& AssetLoader<T>::load(const string& filename)
{
	return load(filename, filename);
}

template<class T>
void AssetLoader<T>::setBasePath(const string& path)
{
	m_basePath = path;
}

