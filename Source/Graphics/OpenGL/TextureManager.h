#pragma once
#include <unordered_map>
#include <GL/glew.h>

class TextureManager
{
public:
	GLuint loadTexture(const std::string& path);
private:
	std::unordered_map<std::string, GLuint> m_textures;
};