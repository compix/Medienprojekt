#include "SystemUtils.h"

using std::string;

#ifdef WIN32
	#include <windows.h>
	#include <direct.h>
#else
	#include <unistd.h>
#endif

namespace SystemUtils {
#ifdef WIN32
	string getExecutableDirectory()
	{
		char buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		string::size_type pos = string(buffer).find_last_of("\\/");
		return string(buffer).substr(0, pos);
	}
	void setCwd(const string &dir)
	{
		_chdir(dir.c_str());
	}
#else
	string getExecutableDirectory()
	{
		char arg1[20];
		char exepath[PATH_MAX + 1] = { 0 };

		sprintf(arg1, "/proc/%d/exe", getpid());
		readlink(arg1, exepath, 1024);
		return string(exepath);
	}
	void setCwd(const string &dir)
	{
		chdir(dir.c_str());
	}
#endif
}