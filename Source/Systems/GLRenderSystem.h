#pragma once
#include <entityx/entityx.h>
#include "../Graphics/OpenGL/TextureManager.h"
#include "../Graphics/OpenGL/MeshManager.h"
#include "../Graphics/OpenGL/Camera.h"
#include "../Graphics/OpenGL/GLShaderManager.h"

using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class GLRenderSystem : public System<GLRenderSystem>
{
public:
	GLRenderSystem();

	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;

private:
	void handleRenderRequests(EntityManager& entityManager);

	void render(EntityManager& entityManager);

	void moveCamera(float dt);
private:
	MeshManager m_meshManager;
	GLShaderManager m_shaderManager;
	TextureManager m_textureManager;

	bool m_mousePressed;
	glm::vec2 m_lastMousePos;

	Camera m_camera;
};
