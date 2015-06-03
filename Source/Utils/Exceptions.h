#pragma once
#include <exception>

using std::exception;
using std::string;

class file_not_found : public exception
{
public:
	file_not_found(const string& path) : m_path(path) {}

	virtual const char* what() const throw() override
	{
		string s = "File " + m_path + " not found.";
		return s.c_str();
	}

private:
	string m_path;
};

class not_supported_function : public exception
{
public:
	not_supported_function(const string& functionName) : m_functionName(functionName) {}
	not_supported_function() : m_functionName("") {}

	virtual const char* what() const throw() override
	{
		string s = "Function " + m_functionName + " not supported.";
		return s.c_str();
	}

private:
	string m_functionName;
};