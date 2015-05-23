#pragma once

enum class ShaderType
{
	SFML,
	LIGHTMAP
};

struct ShaderComponent
{
	ShaderComponent(ShaderType type) : type(type) {}
	ShaderComponent() : type(ShaderType::SFML) {}

	ShaderType type;
};