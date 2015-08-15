#pragma once
#include "../ShaderData.h"
#include <glm/mat4x2.hpp>

class Camera;

class Effect
{
public:
	Effect();
	explicit Effect(const ShaderData& shaderData);

	void apply();

	void setPointLightCount(GLuint num);

	void setCamera(Camera& camera);

	void setShadowCamera(Camera& camera);

	void setModel(const glm::mat4& modelMatrix);

	inline void setShaderData(const ShaderData& shaderData) { m_shaderData = shaderData; }

	inline bool hasSameProgram(GLuint shaderProgram) { return m_shaderData.shaderProgram == shaderProgram; }

	// void setPointLight(const PointLight& pointLight, int n);
	// void setMaterial(const Material& material);
	// void setDirLight(const DirectionalLight& dirLight);
private:
	ShaderData m_shaderData;
};