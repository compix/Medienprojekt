#include "GLShaderManager.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>

GLShaderManager::GLShaderManager()
{
}

ShaderData GLShaderManager::loadShaderProgram(ShaderType type)
{
	if (m_shaderPrograms.count(type))
		return m_shaderPrograms[type];

	switch (type)
	{
	case ShaderType::BASIC:
		m_shaderPrograms[type] = load("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
		return m_shaderPrograms[type];
	case ShaderType::BILLBOARD: 
		m_shaderPrograms[type] = load("Assets/Shaders/billboard.vert", "Assets/Shaders/basic.frag");
		return m_shaderPrograms[type];
		break;
	default:
		assert(false);
	}

	return ShaderData();
}

GLShaderManager::~GLShaderManager()
{
	for (auto& programEntry : m_shaderPrograms)
	{
		glDeleteProgram(programEntry.second.shaderProgram);
	}
}

ShaderData GLShaderManager::load(const std::string& vsPath, const std::string& fsPath)
{
	GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

	// Compile Vertex Shader
	std::string source = readFileAsString(vsPath);
	char const * vsSource = source.c_str();
	glShaderSource(vsID, 1, &vsSource, nullptr);
	glCompileShader(vsID);

	shaderErrorCheck(vsID, vsPath);

	// Compile Fragment Shader
	source = readFileAsString(fsPath);
	char const * fsSource = source.c_str();
	glShaderSource(fsID, 1, &fsSource, nullptr);
	glCompileShader(fsID);

	shaderErrorCheck(fsID, fsPath);

	// Link the program
	GLuint programID = glCreateProgram();

	glAttachShader(programID, vsID);
	glAttachShader(programID, fsID);

	glLinkProgram(programID);

	programErrorCheck(programID, vsPath, fsPath);

	glDeleteShader(vsID);
	glDeleteShader(fsID);

	ShaderData shaderData(programID);
	
	return shaderData;
}

std::string GLShaderManager::readFileAsString(const std::string& path)
{
	std::string fileAsString;
	std::ifstream stream(path, std::ios::in);

	if (stream.is_open())
	{
		std::string line = "";
		while (getline(stream, line))
			fileAsString += "\n" + line;

		stream.close();
	}

	return fileAsString;
}

void GLShaderManager::shaderErrorCheck(GLuint shader, const std::string& shaderPath)
{
	GLint result = GL_FALSE;
	GLint infoLogLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> shaderLog(std::max(infoLogLength, 1));
	glGetShaderInfoLog(shader, infoLogLength, nullptr, &shaderLog[0]);

	if (result == GL_FALSE)
		std::cout << "Shader comilation failed for " << shaderPath << std::endl;

	fprintf(stdout, "%s\n", &shaderLog[0]);
}

void GLShaderManager::programErrorCheck(GLuint program, const std::string& vsPath, const std::string& fsPath)
{
	GLint result = GL_FALSE;
	GLint infoLogLength;

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> programLog(std::max(infoLogLength, 1));
	glGetProgramInfoLog(program, infoLogLength, nullptr, &programLog[0]);


	if (result == GL_FALSE)
		std::cout << "Linking the program failed for " << vsPath << " and " << fsPath << std::endl;

	fprintf(stdout, "%s\n", &programLog[0]);
}