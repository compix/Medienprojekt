#pragma once
#include "AssetLoader.h"
#include "../../AI/Personality/AIPersonality.h"

class AILoader : public AssetLoader<AIPersonality>
{
public:
	AILoader() :AssetLoader("AI", "") {}

protected:
	inline bool preload(const string& key, const string& filename) override { return false; };

	bool load(const string& key, const Json::Value& jsonValue) override;
};