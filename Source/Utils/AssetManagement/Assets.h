#pragma once

namespace Assets
{
	class Texture
	{
	public:
		Texture() {}
		Texture(sf::Texture* tex, sf::IntRect rect) : m_tex(tex), m_textureRect(rect) {}
		inline const sf::IntRect& getRect() const { return m_textureRect; }
		inline const sf::Texture* get() const { return m_tex; }
	private:
		sf::IntRect m_textureRect;
		sf::Texture* m_tex;
	};
}