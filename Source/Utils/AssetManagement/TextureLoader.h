#pragma once
#include <SFML/Graphics/Texture.hpp>
#include "AssetLoader.h"

using sf::Texture;

class TextureLoader : public AssetLoader<Texture>
{
public:
	TextureLoader() : AssetLoader("Textures", "Assets/textures/") {}

protected:
	bool preload(const string& key, const string &filename) override;
	bool load(const string& key, const Json::Value& jsonValue) override;

public:
	inline AssetMap getTextureMap() { return m_assets; }
};