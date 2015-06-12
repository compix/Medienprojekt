#include "AssetManager.h"

AssetManager::AssetManager()
{
	m_textureLoader.loadAllFromJson("Assets/json/textures.json");
	m_animationLoader.loadAllFromJson("Assets/json/animations.json");
	m_soundLoader.loadAllFromJson("Assets/json/sound.json");
	m_musicLoader.loadAllFromJson("Assets/json/music.json");

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

Assets::Animation* AssetManager::getAnimation(const std::string& name)
{
	return &m_animationLoader.get(name);
}

SoundBuffer* AssetManager::getSound(const std::string& name)
{
	return &m_soundLoader.get(name);
}

Music* AssetManager::getMusic(const std::string& name)
{
	return &m_musicLoader.get(name);
}