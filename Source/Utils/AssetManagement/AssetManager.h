#pragma once
#include "TextureLoader.h"
#include "Assets.h"
#include "TexturePacker.h"

class AssetManager
{
public:
	AssetManager();

	Assets::Texture* getTexture(const std::string& name);
private:
	TextureLoader m_textureLoader;
	TexturePacker m_texturePacker;

	std::unordered_map<std::string, Assets::Texture> m_textures;
	std::vector<std::shared_ptr<sf::Texture>> m_uniqueTextures;
};