#pragma once
#include "../../Components/AnimationComponent.h"

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

	struct Animation
	{
		Animation() : colCount(0), rowCount(0), frameCount(0), frameDuration(1.f), startFrame(0), playMode(NORMAL) {}

		uint8_t colCount;
		uint8_t rowCount;
		uint8_t frameCount;
		float frameDuration;
		uint8_t startFrame;
		PlayMode playMode;
	};
}