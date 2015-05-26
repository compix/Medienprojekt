#include <SFML/Graphics/Sprite.hpp>
#include "TexturePacker.h"

// Checks if r1 and r2 overlap
bool overlaps(const sf::IntRect& r1, const sf::IntRect& r2)
{
	return (r1.left + r1.width > r2.left) && (r1.left < r2.left + r2.width) &&
		   (r1.top + r1.height > r2.top) && (r1.top < r2.top + r2.height);
}

// Checks if r1 is inside r2
bool inside(const sf::IntRect& r1, const sf::IntRect& r2)
{
	return r1.left >= r2.left && r1.left + r1.width <= r2.left + r2.width &&
		   r1.top >= r2.top && r1.top + r1.height <= r2.top + r2.height;
}

bool TexturePacker::addTexture(sf::Texture texture, std::string name)
{
	if (texture.getSize().x > MAX_TEXTURE_WIDTH || texture.getSize().y > MAX_TEXTURE_HEIGHT)
		return false;

	m_textures.push_back(NamedTexture(texture, name));
	return true;
}

/**
 * @brief	Simple scanline texture packing algorithm. The size of the atlases is currently always at the maximum
 * 			and might be changed if necessary.
 */
void TexturePacker::pack(std::vector<std::shared_ptr<sf::Texture>>& uniqueTextures, std::unordered_map<std::string, Assets::Texture>& textures)
{
	if (m_textures.size() == 0)
		return;

	createPackedTexture(MAX_TEXTURE_WIDTH, MAX_TEXTURE_HEIGHT);

	sf::IntRect packedArea(0, 0, MAX_TEXTURE_WIDTH, MAX_TEXTURE_HEIGHT);

	auto largeToSmall = [](const NamedTexture& t1, const NamedTexture& t2)
	{
		return t1.texture.getSize().x*t1.texture.getSize().y > t2.texture.getSize().x*t2.texture.getSize().y;
	};

	std::sort(m_textures.begin(), m_textures.end(), largeToSmall);
	
	int textureIndex = 0;
	int packedIndex = 0;
	while (textureIndex < m_textures.size())
	{	
		auto packedTexture = m_packedTextures[packedIndex];

		auto& t = m_textures[textureIndex];
		auto width = t.texture.getSize().x;
		auto height = t.texture.getSize().y;
		sf::IntRect tested(0, 0, width, height);

		// Check if the area is free:
		bool isFree = false;
		while (!isFree && inside(tested, packedArea))
		{
			isFree = true;
			sf::IntRect overlapArea;
			for (auto& area : m_usedAreas[packedIndex])
			{
				isFree = !overlaps(area, tested);

				if (!isFree)
				{
					overlapArea = area;
					break;
				}
					
			}

			if (!isFree)
			{
				tested.left += overlapArea.width;
				if (!inside(tested, packedArea))
				{
					tested.left = 0;
					tested.top += overlapArea.height;
				}
			}
		}

		if (isFree)
		{
			packedTexture->m_textureAreas[t.name] = tested;
			sf::Sprite sprite(t.texture);
			sprite.setPosition(tested.left, tested.top);
			packedTexture->m_renderTexture.draw(sprite);
			m_usedAreas[packedIndex].push_back(tested);
			++textureIndex;

			packedIndex = 0;
		}
		else
		{
			// Doesn't fit here so check the next
			++packedIndex;
			if (packedIndex >= m_packedTextures.size())
			{
				// Need a new packed texture
				createPackedTexture(MAX_TEXTURE_WIDTH, MAX_TEXTURE_HEIGHT);
			}

			packedTexture = m_packedTextures[packedIndex];
		}
	}

	for (int i = 0; i < m_packedTextures.size(); ++i)
	{
		sf::Texture tex = m_packedTextures[i]->getTexture();
		uniqueTextures.push_back(std::make_shared<sf::Texture>(tex));
		auto uniqueTexture = uniqueTextures[uniqueTextures.size() - 1].get();

		for (auto& area : m_packedTextures[i]->m_textureAreas)
			textures[area.first] = Assets::Texture(uniqueTexture, area.second);
	}
}

void TexturePacker::render(sf::RenderTarget& renderTarget)
{
	float x = 0;
	float y = 0;

	float scaleX = 0.2f;
	float scaleY = 0.2f;

	for (int i = 0; i < m_packedTextures.size(); ++i)
	{
		const sf::Texture& tex = m_packedTextures[i]->m_renderTexture.getTexture();
		sf::Sprite sprite = sf::Sprite(tex);
		sprite.setPosition(x, y);
		sprite.setScale(scaleX, scaleY);
		renderTarget.draw(sprite);

		x += tex.getSize().x*scaleX;
		if (x >= 1680)
		{
			x = 0.f;
			y += tex.getSize().y*scaleY;
		}
	}
}

PackedTexture* TexturePacker::createPackedTexture(int width, int height)
{
	m_packedTextures.push_back(std::make_shared<PackedTexture>(width, height));
	m_usedAreas.push_back(std::vector<sf::IntRect>());
	return m_packedTextures[m_packedTextures.size() - 1].get();
}

PackedTexture::PackedTexture(int width, int height)
{
	m_renderTexture.create(width, height);
	m_renderTexture.clear(sf::Color::Transparent);
}