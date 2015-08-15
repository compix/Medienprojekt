#include "Effect.h"
#include "../Camera.h"

Effect::Effect()
	:m_shaderData(ShaderData(0))
{
}

Effect::Effect(const ShaderData& shaderData)
	:m_shaderData(shaderData)
{
}

void Effect::apply()
{
	glUseProgram(m_shaderData.shaderProgram);
}

void Effect::setPointLightCount(GLuint num)
{
	glUniform1i(glGetUniformLocation(m_shaderData.shaderProgram, "pointLightCount"), num);
}

void Effect::setCamera(Camera& camera)
{
	glm::mat4 test = camera.m_view;
	glUniformMatrix4fv(glGetUniformLocation(m_shaderData.shaderProgram, "view"), 1, GL_FALSE, &camera.view()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderData.shaderProgram, "proj"), 1, GL_FALSE, &camera.proj()[0][0]);
}

void Effect::setShadowCamera(Camera& camera)
{
	glUniformMatrix4fv(glGetUniformLocation(m_shaderData.shaderProgram, "shadowView"), 1, GL_FALSE, &camera.view()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_shaderData.shaderProgram, "shadowProj"), 1, GL_FALSE, &camera.proj()[0][0]);
}

void Effect::setModel(const glm::mat4& modelMatrix)
{
	glUniformMatrix4fv(glGetUniformLocation(m_shaderData.shaderProgram, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
}