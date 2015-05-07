#pragma once
#include "AssetLoader.h"
#include <SFML/Graphics/Texture.hpp>

using sf::Texture;

class TextureLoader : public AssetLoader<Texture>
{
public:
	TextureLoader();
	TextureLoader(string basePath);

	virtual Texture& load(const string& filename, const string& name) override;
};