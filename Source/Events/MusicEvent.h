#pragma once
#include <string>

using std::string;
struct MusicEvent
{
	explicit MusicEvent(const string &name, bool loop = false) : loop(loop), name(name){}

	bool loop;
	string name;
};
