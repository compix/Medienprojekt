#include "GLRenderSystem.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "../Utils/Math.h"
#include "../Components/GLRenderRequestComponent.h"
#include "../Components/GLRenderComponent.h"
#include "../Graphics/OpenGL/Effects/Effect.h"
#include "../GameConstants.h"
#include <glm/ext.hpp>
#include "../GameGlobals.h"
#include "SFML/Graphics.hpp"
#include "../Components/TransformComponent.h"
#include "../Components/CellComponent.h"

GLRenderSystem::GLRenderSystem()
{
	m_camera.setPerspective(45, 800.f / 600.f, 0.5f, 100.0f);
	m_camera.setPosition(glm::vec3(5, 8, 10));
	m_camera.lookAt(glm::vec3(5, 8, 10), glm::vec3(5, 0, 5), glm::vec3(0, 1, 0));
}

void GLRenderSystem::update(EntityManager& entityManager, EventManager& eventManager, TimeDelta dt)
{
	GameGlobals::window->setActive();
	moveCamera(static_cast<float>(dt));

	handleRenderRequests(entityManager);

	render(entityManager);

	GameGlobals::window->resetGLStates();
}

void GLRenderSystem::handleRenderRequests(EntityManager& entityManager)
{
	for (auto entity : entityManager.entities_with_components<GLRenderRequestComponent>())
	{
		assert(!entity.has_component<GLRenderComponent>());

		auto renderRequest = entity.component<GLRenderRequestComponent>();

		ShaderData shaderData = m_shaderManager.loadShaderProgram(renderRequest->shaderType);
		Mesh mesh = m_meshManager.load(renderRequest->meshType, renderRequest->vertexType);
		glm::vec3 scale = renderRequest->scale;
		GLuint texture = m_textureManager.loadTexture(renderRequest->texturePath);

		entity.assign<GLRenderComponent>(mesh, shaderData, texture, scale);
		entity.remove<GLRenderRequestComponent>();
	}
}

void GLRenderSystem::render(EntityManager& entityManager)
{
	m_camera.updateViewMatrix();

	glClearColor(146.f / 255, 138.f / 255, 138.f / 255, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//glViewport(0, 0, 800, 600);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	Effect effect;

	for (auto entity : entityManager.entities_with_components<TransformComponent, CellComponent, GLRenderComponent>())
	{
		auto renderComponent = entity.component<GLRenderComponent>();
		auto transform = entity.component<TransformComponent>();
		auto cell = entity.component<CellComponent>();

		if (!effect.hasSameProgram(renderComponent->shaderData.shaderProgram))
		{
			effect.setShaderData(renderComponent->shaderData);
			effect.apply();

			effect.setCamera(m_camera);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderComponent->texID);

		glm::mat4 scale = glm::scale(renderComponent->scale);
		glm::mat4 translation = glm::translate(glm::vec3(transform->x / 50.f, 0, transform->y / 50.f));
		glm::mat4 model = translation * scale;

		effect.setModel(model);

		glBindVertexArray(renderComponent->mesh.vao);

		switch (renderComponent->mesh.renderMode)
		{
		case RenderMode::TRIANGLES_INDEXED:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderComponent->mesh.ibo);
			glDrawElements(GL_TRIANGLES, renderComponent->mesh.indexCount, GL_UNSIGNED_INT, nullptr);
			break;
		case RenderMode::TRIANGLE_STRIP:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, renderComponent->mesh.vertexCount);
			break;
		default:
			assert(false);
		}


	}

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLRenderSystem::moveCamera(float dt)
{
	float speed = 2.f * dt;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_camera.walk(-speed);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_camera.walk(speed);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_camera.strafe(-speed);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_camera.strafe(speed);

	if (!m_mousePressed && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2i position = sf::Mouse::getPosition();
		m_lastMousePos = glm::vec2(position.x, position.y);
	}

	m_mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

	if (m_mousePressed) {
		sf::Vector2i position = sf::Mouse::getPosition();

		float dx = static_cast<float>(position.x - m_lastMousePos.x);
		float dy = static_cast<float>(position.y - m_lastMousePos.y);

		m_camera.pitch((float)Math::toRadians(-dy * 0.1f));
		m_camera.rotateY((float)Math::toRadians(-dx * 0.1f));

		m_lastMousePos = glm::vec2(position.x, position.y);
	}
}