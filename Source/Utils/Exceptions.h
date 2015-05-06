#pragma once
#include <exception>

using std::exception;
using std::string;

class file_not_found : public exception
{
public:
	file_not_found(string path) : m_path(path) {}

	virtual const char* what() const throw() override
	{
		string s = "File " + m_path + " not found.";
		return s.c_str();
	}

private:
	string m_path;
};