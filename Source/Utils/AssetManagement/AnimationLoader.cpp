#include "AnimationLoader.h"

bool AnimationLoader::load(const string& key, const Json::Value& jsonValue)
{
	Assets::Animation animation;

	for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it)
	{
		auto jsKey = it.key().asString();
		if (jsKey == "colCount")
			animation.colCount = it->asUInt();
		else if (jsKey == "rowCount")
			animation.rowCount = it->asUInt();
		else if (jsKey == "frameCount")
			animation.frameCount = it->asUInt();
		else if (jsKey == "frameDuration")
			animation.frameDuration = it->asFloat();
		else if (jsKey == "startFrame")
			animation.startFrame = it->asUInt();
		else if (jsKey == "playMode")
		{
			auto playMode = it->asString();
			if (playMode == "NORMAL")
				animation.playMode = NORMAL;
			else if (playMode == "LOOP")
				animation.playMode = LOOP;
			else if (playMode == "REVERSED")
				animation.playMode = REVERSED;
			else if (playMode == "LOOP_REVERSED")
				animation.playMode = LOOP_REVERSED;
			else
				cout << "AnimationLoader - WARNING: Unrecognized PlayMode: " << playMode << endl;
		}
		else
			cout << "AnimationLoader - WARNING: Unrecognized JsonKey: " << it.key() << endl;
	}

	m_assets[key] = animation;
	return true;
}