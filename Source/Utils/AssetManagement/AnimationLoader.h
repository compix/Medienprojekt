#pragma once
#include "AssetLoader.h"
#include "../Exceptions.h"
#include "Assets.h"

class AnimationLoader : public AssetLoader<Assets::Animation>
{
public:
	Assets::Animation& load(const string& filename, const string& name) override { throw not_supported_function("load(string, string)"); };
	Assets::Animation& load(const string& name, const Json::Value& jsonValue) override;

};