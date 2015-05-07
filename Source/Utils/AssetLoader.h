#pragma once
#include <unordered_map>
#include <string>
#include "json/json.h"
#include <fstream>
#include <iostream>
#include "Exceptions.h"

using std::unordered_map;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

template<class T>
class AssetLoader
{
	typedef unordered_map<string, T> AssetMap;
public:

	virtual ~AssetLoader() { };

	virtual bool loadAllFromJson(const string& path);
	virtual T& load(const string& filename);
	virtual T& load(const string& filename, const string& name) = 0;
	virtual T& get(const string& name);

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

	if(!ifs.is_open()) {
		cerr << "Can't find file: " << path << endl;
		throw file_not_found(path);
	}
	
	if (!reader.parse(ifs, root, false))
	{
		cerr << "Parsing error: " << reader.getFormattedErrorMessages() << endl;
		return false;
	}

	for (auto it = root.begin(); it != root.end(); it++)
	{
		load(it->asString(), it.key().asString());
	}

	return true;
}

template<class T>
T& AssetLoader<T>::get(const string& name)
{
	return m_assets.at(name);
}

template<class T>
T& AssetLoader<T>::load(const string& filename)
{
	return load(filename, filename);
}

template<class T>
void AssetLoader<T>::setBasePath(const string& path)
{
	m_basePath = path;
}

