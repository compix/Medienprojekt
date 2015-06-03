#pragma once
#include <SFML/Graphics/Shader.hpp>
#include <memory>

class ShaderManager
{

public:
	ShaderManager();

	inline sf::Shader* getLightShader() { return &m_lightShader; }

	inline void updateScreenResolution(const sf::Vector2u& screenResolution)
	{
		m_lightShader.setParameter("screenResolution", sf::Vector2f((float)screenResolution.x, (float)screenResolution.y));
	}

private:
	sf::Shader m_lightShader;
};