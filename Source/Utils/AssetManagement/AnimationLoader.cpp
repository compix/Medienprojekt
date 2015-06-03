#include "AnimationLoader.h"

Assets::Animation& AnimationLoader::load(const string& name, const Json::Value& jsonValue)
{
	Assets::Animation animation;

	for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it)
	{
		auto key = it.key().asString();
		if (key == "colCount")
			animation.colCount = it->asUInt();
		else if (key == "rowCount")
			animation.rowCount = it->asUInt();
		else if (key == "frameCount")
			animation.frameCount = it->asUInt();
		else if (key == "frameDuration")
			animation.frameDuration = it->asFloat();
		else if (key == "startFrame")
			animation.startFrame = it->asUInt();
		else if (key == "playMode")
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

	m_assets[name] = animation;
	return m_assets[name];
}