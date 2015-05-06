#pragma once

#include <string>

namespace SystemUtils
{
	using std::string;
	string getExecutableDirectory();
	void setCwd(const string &dir);
}

