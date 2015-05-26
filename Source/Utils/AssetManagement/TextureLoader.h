#pragma once
#include <SFML/Graphics/Texture.hpp>
#include "AssetLoader.h"

using sf::Texture;

class TextureLoader : public AssetLoader<Texture>
{
public:
	TextureLoader();
	TextureLoader(string basePath);

	virtual Texture& load(const string& filename, const string& name) override;

	inline AssetMap getTextureMap() { return m_assets; }
};