#pragma once
#include "AssetLoader.h"

class TextureLoader : public AssetLoader<Texture>
{
public:
	TextureLoader();
	TextureLoader(string basePath);

	virtual Texture& load(const string& filename, const string& name) override;
};