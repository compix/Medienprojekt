#pragma once
#include <ecstasy/core/Component.h>

enum class ShaderType
{
	SFML,
	LIGHTMAP
};

struct ShaderComponent: public Component<ShaderComponent>
{
	ShaderComponent(ShaderType type) : type(type) {}
	ShaderComponent() : type(ShaderType::SFML) {}

	ShaderType type;
};