#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <unordered_map>
#include <assert.h>
#include "Assets.h"
#include <memory>

const unsigned short MAX_TEXTURE_WIDTH = 2048;
const unsigned short MAX_TEXTURE_HEIGHT = 2048;

class PackedTexture
{
	friend class TexturePacker;
public:
	PackedTexture(int width, int height);

	inline const sf::Texture& getTexture() { m_renderTexture.display(); return m_renderTexture.getTexture(); }
private:
	std::unordered_map<std::string, sf::IntRect> m_textureAreas;
	sf::RenderTexture m_renderTexture;
};

class TexturePacker
{
	friend class AssetManager;
	struct NamedTexture
	{
		NamedTexture(sf::Texture texture, std::string name) : texture(texture), name(name) {}
		sf::Texture texture;
		std::string name;
	};
public:
	bool addTexture(sf::Texture texture, std::string name);
	void pack(std::vector<std::shared_ptr<sf::Texture>>& uniqueTextures, std::unordered_map<std::string, Assets::Texture>& textures);

	void render(sf::RenderTarget& renderTarget);
private:
	PackedTexture* createPackedTexture(int width, int height);
private:
	std::vector<std::shared_ptr<PackedTexture>> m_packedTextures;
	std::vector<NamedTexture> m_textures;
	std::vector<std::vector<sf::IntRect>> m_usedAreas; // same number as packed textures
};