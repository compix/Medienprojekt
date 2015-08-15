#pragma once
#include <GL/glew.h>
#include "GeometryGenerator.h"


struct Mesh
{
	GLuint vbo;
	GLuint ibo;
	GLuint vao;

	GLuint indexCount;
	GLuint vertexCount;

	RenderMode renderMode;
};