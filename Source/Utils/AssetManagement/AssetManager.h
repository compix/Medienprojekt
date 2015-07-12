#pragma once
#include "TextureLoader.h"
#include "Assets.h"
#include "TexturePacker.h"
#include "AnimationLoader.h"
#include "SoundLoader.h"
#include <SFML/Audio.hpp>
#include "MusicLoader.h"

const uint8_t MAX_SOUNDS = 254;

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	Assets::Texture* getTexture(const std::string& name);
	Assets::Animation* getAnimation(const std::string& name);
	sf::Sound* getSound(const std::string& name);
	Music* getMusic(const std::string& name);

	bool preloadsDone() { return m_preloadsDone == m_preloadCount; }

	void preloadNext();
	void emitPreloadEvent();

private:
	TextureLoader m_textureLoader;
	TexturePacker m_texturePacker;
	AnimationLoader m_animationLoader;
	SoundLoader m_soundLoader;
	MusicLoader m_musicLoader;
	std::deque<AssetLoaderBase *> m_loaders;

	std::unordered_map<std::string, Assets::Texture> m_textures;
	std::vector<std::shared_ptr<sf::Texture>> m_uniqueTextures;

	sf::Sound m_sounds[MAX_SOUNDS];
	int m_preloadCount = 0;
	int m_preloadsDone = 0;
};