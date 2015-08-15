#pragma once
#include "../Graphics/OpenGL/Mesh.h"
#include <glm/detail/type_vec3.hpp>
#include "../Graphics/OpenGL/ShaderData.h"

struct GLRenderComponent
{
	GLRenderComponent() {}

	GLRenderComponent(const Mesh& mesh, const ShaderData& shaderData, GLuint texID, const glm::vec3& scale)
		:mesh(mesh), shaderData(shaderData), texID(texID), scale(scale) {}

	Mesh mesh;
	ShaderData shaderData;
	GLuint texID;

	glm::vec3 scale;
};