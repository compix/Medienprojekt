#include "Shaders.h"
#include <iostream>

ShaderManager::ShaderManager()
{
	if (!m_lightShader.loadFromFile("Assets/shaders/lightShader.vert", "Assets/shaders/lightShader.frag"))
		std::cerr << "Could not load lightShader." << std::endl;
}