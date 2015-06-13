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

AssetManager::~AssetManager()
{
	for (auto& s : m_sounds)
		s.resetBuffer();
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

sf::Sound* AssetManager::getSound(const std::string& name)
{
	for (auto i = 0; i < MAX_SOUNDS; ++i)
	{
		if (m_sounds[i].getStatus() == sf::SoundSource::Stopped)
		{
			m_sounds[i] = sf::Sound(m_soundLoader.get(name));
			return &m_sounds[i];
		}
	}

	// Too many sounds
	assert(false);
	return nullptr;
}

Music* AssetManager::getMusic(const std::string& name)
{
	return &m_musicLoader.get(name);
}