#include "TextureLoader.h"

TextureLoader::TextureLoader()
{
	m_basePath = "Assets/textures/";
}

TextureLoader::TextureLoader(std::string basePath)
{
	m_basePath = basePath;
}

Texture& TextureLoader::load(const string& filename, const string& name)
{
	Texture texture;
	string path = m_basePath + filename;
	if (!texture.loadFromFile(path))
		throw file_not_found(path);

	m_assets[name] = texture;
	return m_assets[name];
}

