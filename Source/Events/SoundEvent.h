#pragma once
#include <string>

using std::string;
struct SoundEvent
{
	explicit SoundEvent(const string &name) :name(name){}

	string name;
};

