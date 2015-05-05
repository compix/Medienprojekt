#include "SystemUtils.h"

using std::string;

#ifdef WIN32
	#include <windows.h>
	#include <direct.h>
	#define PATH_MAX MAX_PATH
#else
	#include <unistd.h>
	#include <linux/limits.h>
#endif

namespace SystemUtils {
	string getExecutableDirectory()
	{
		char buffer[PATH_MAX + 1] = { 0 };
#ifdef WIN32
		GetModuleFileName(NULL, buffer, PATH_MAX);
#else
		char linkPath[20];
		snprintf(linkPath, 20, "/proc/%d/exe", getpid());
		readlink(linkPath, buffer, PATH_MAX);
#endif
		string::size_type pos = string(buffer).find_last_of("\\/");
		return string(buffer).substr(0, pos);
	}
	void setCwd(const string &dir)
	{
#ifdef WIN32
		_chdir(dir.c_str());
#else
		chdir(dir.c_str());
#endif
	}
}