#pragma once
#include "AssetLoader.h"
#include "../Exceptions.h"
#include "Assets.h"

class AnimationLoader : public AssetLoader<Assets::Animation>
{
public:
	AnimationLoader() : AssetLoader("Animations", "") {}

protected:
	bool preload(const string& key, const string &filename) override { return false; };
	bool load(const string& key, const Json::Value& jsonValue) override;
};