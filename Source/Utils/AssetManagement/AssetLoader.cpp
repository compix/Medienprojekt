#include "AssetLoader.h"
#include "../../Events/ExitEvent.h"
#include "../../Game.h"

bool AssetLoaderBase::loadFromJson(const string& path)
{
	Json::Value root;
	Json::Reader reader;
	ifstream ifs(path, ifstream::binary);

	if (!ifs.is_open()) {
		cerr << "Can't find file: " << path << endl;
		return false;
	}

	if (!reader.parse(ifs, root, false))
	{
		cerr << "Parsing error: " << reader.getFormattedErrorMessages() << endl;
		return false;
	}

	for (auto it = root.begin(); it != root.end(); ++it)
	{
		load(it.key().asString(), *it);
	}

	return true;
}

bool AssetLoaderBase::preloadNext()
{
	auto & entry = m_preloads.front();

	bool success = preload(entry.key, entry.filename);
	if (!success)
	{
		cerr << "Could not load file: " << entry.filename << endl;
		GameGlobals::events->emit<ExitEvent>();
	}
	m_preloads.pop_front();
	return success;
}

void AssetLoaderBase::addPreload(const std::string& key, const std::string& filename)
{
	m_preloads.push_back({ key, filename });
}

const std::string& AssetLoaderBase::getNextFilename()
{
	auto & entry = m_preloads.front();
	return entry.filename;
}
