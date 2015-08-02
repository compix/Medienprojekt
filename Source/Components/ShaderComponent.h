#pragma once
#include <ecstasy/core/Component.h>

enum class ShaderType
{
	SFML,
	LIGHTMAP
};

struct ShaderComponent: public ECS::Component<ShaderComponent>
{
	ShaderType type = ShaderType::SFML;
	
	void reset() override {
		type = ShaderType::SFML;
	}
};