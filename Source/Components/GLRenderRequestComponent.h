#pragma once
#include "../Graphics/OpenGL/MeshManager.h"
#include "../Graphics/OpenGL/GLShaderManager.h"

/**
* Used to remove OpenGL rendering dependencies.
*/
struct GLRenderRequestComponent
{
	GLRenderRequestComponent() {}

	GLRenderRequestComponent(MeshType meshType, VertexType vertexType, ShaderType shaderType, const std::string& texturePath, const glm::vec3& scale)
		:meshType(meshType), vertexType(vertexType), shaderType(shaderType), texturePath(texturePath), scale(scale) {}

	MeshType meshType;
	VertexType vertexType;
	ShaderType shaderType;
	std::string texturePath;

	glm::vec3 scale;
};