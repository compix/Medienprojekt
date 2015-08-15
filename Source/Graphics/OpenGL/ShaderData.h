#pragma once
#include <GL/glew.h>

struct ShaderData
{
	ShaderData() {}
	ShaderData(GLuint shaderProgram) : shaderProgram(shaderProgram) {}

	GLuint shaderProgram;
};