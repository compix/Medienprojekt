#include "AILoader.h"

bool AILoader::load(const string& key, const Json::Value& jsonValue)
{
	AIPersonality personality;
	personality.name = key;

	for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it)
	{
		auto jsKey = it.key().asString();

		AIPersonality::AIDesires* desires = nullptr;
		if (jsKey == "affinity")
			desires = &personality.affinity;
		else if (jsKey == "desires")
			desires = &personality.desires;
		else if (jsKey == "changePerSecond")
			desires = &personality.changePerSecond;
		else if (jsKey == "favoriteSkill")
		{
			auto skillArray = *it;
			for (size_t i = 0; i < it->size(); ++i)
			{
				auto skillType = skillArray[i].asString();

				if (skillType == "PLACE_PORTAL")
					personality.addFavorite(SkillType::PLACE_PORTAL);
				else if (skillType == "PUNCH")
					personality.addFavorite(SkillType::PUNCH);
				else if (skillType == "BLINK")
					personality.addFavorite(SkillType::BLINK);
			}

			continue;
		}
		else
		{
			cout << "AILoader - WARNING: Unexpected token: " << jsKey << endl;
			continue;
		}

		for (auto desireIt = it->begin(); desireIt != it->end(); ++desireIt)
		{
			auto desireKey = desireIt.key().asString();

			if (desireKey == "destroyBlock")
				desires->destroyBlock = desireIt->asFloat();
			else if (desireKey == "wait")
				desires->wait = desireIt->asFloat();
			else if (desireKey == "attackEnemy")
				desires->attackEnemy = desireIt->asFloat();
			else if (desireKey == "placePortal")
				desires->placePortal = desireIt->asFloat();
			else if (desireKey == "getItem")
				desires->getItem = desireIt->asFloat();
			else if (desireKey == "getSafe")
				desires->getSafe = desireIt->asFloat();
			else
			{
				cout << "AILoader - WARNING: Unexpected token: " << desireKey << endl;
				continue;
			}
		}
	}

	m_assets[key] = personality;
	return true;
}