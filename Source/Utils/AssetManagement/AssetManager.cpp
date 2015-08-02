#include "AssetManager.h"
#include "../../Game.h"


AssetManager::AssetManager()
{
	m_textureLoader.loadFromJson("Assets/json/textures.json");
	m_animationLoader.loadFromJson("Assets/json/animations.json");
	m_soundLoader.loadFromJson("Assets/json/sound.json");
	m_musicLoader.loadFromJson("Assets/json/music.json");

	m_loaders.push_back(&m_textureLoader);
	m_loaders.push_back(&m_animationLoader);
	m_loaders.push_back(&m_soundLoader);
	m_loaders.push_back(&m_musicLoader);

	for (auto *loader : m_loaders)
		m_preloadCount += loader->getPreloadCount();

	emitPreloadEvent();
}

AssetManager::~AssetManager()
{
	for (auto& s : m_sounds)
		s.resetBuffer();
}

Assets::Texture* AssetManager::getTexture(const std::string& name)
{
	if (m_textures.count(name) == 0)
		cerr << "Texture not found: " << name << endl;
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

void AssetManager::preloadNext()
{
	if (m_loaders.empty())
		return;

	auto *loader = m_loaders.front();
	if (!loader->preloadsDone())
	{
		loader->preloadNext();
		m_preloadsDone++;
	}
	if (loader->preloadsDone())
		m_loaders.pop_front();

	if (m_loaders.empty())
	{
		for (auto t : m_textureLoader.getTextureMap())
		{
			m_texturePacker.addTexture(t.second, t.first);
		}

		m_texturePacker.pack(m_uniqueTextures, m_textures);
	}

	emitPreloadEvent();
}

void AssetManager::emitPreloadEvent()
{
	while (!m_loaders.empty())
	{
		auto *loader = m_loaders.front();
		if (loader->preloadsDone())
			m_loaders.pop_front();
		else
			break;
	}

	if (!preloadsDone())
		GameGlobals::events->preload.emit(m_preloadsDone, m_preloadCount, m_loaders.front()->getName(), m_loaders.front()->getNextFilename());
	else
		GameGlobals::events->preload.emit(m_preloadsDone, m_preloadCount, "Game", "Game");
}


Music* AssetManager::getMusic(const std::string& name)
{
	return &m_musicLoader.get(name);
}