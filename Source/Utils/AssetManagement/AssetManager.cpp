#include "AssetManager.h"

AssetManager::AssetManager()
{
	m_textureLoader.loadAllFromJson("Assets/json/textures.json");

	for (auto t : m_textureLoader.getTextureMap())
	{
		m_texturePacker.addTexture(t.second, t.first);
	}

	m_texturePacker.pack(m_uniqueTextures, m_textures);
}


Assets::Texture* AssetManager::getTexture(const std::string& name)
{
	assert(m_textures.count(name) > 0);
	return &m_textures[name];
}