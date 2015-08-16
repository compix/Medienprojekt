#include "TextureManager.h"
#include <SFML/Graphics/Image.hpp>

GLuint TextureManager::loadTexture(const std::string& path)
{
	if (m_textures.count(path))
		return m_textures[path];

	GLuint texture = 0;

	sf::Image image;
	image.loadFromFile(path);
	image.flipVertically();
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.getSize().x, image.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	m_textures[path] = texture;

	return texture;
}

TextureManager::~TextureManager()
{
	for (auto& pair : m_textures)
	{
		glDeleteTextures(1, &pair.second);
	}
}