#pragma once
#include <unordered_map>
#include "ShaderData.h"

enum class ShaderType
{
	BASIC,
	BILLBOARD
};

class GLShaderManager
{
public:
	GLShaderManager();

	ShaderData loadShaderProgram(ShaderType type);

	~GLShaderManager();
private:
	ShaderData load(const std::string& vsPath, const std::string& fsPath);

	std::string readFileAsString(const std::string& path);

	void shaderErrorCheck(GLuint shader, const std::string& shaderPath);
	void programErrorCheck(GLuint program, const std::string& vsPath, const std::string& fsPath);
private:
	std::unordered_map<ShaderType, ShaderData> m_shaderPrograms;
};