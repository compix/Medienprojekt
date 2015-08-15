#include "Camera.h"
#include <glm/ext.hpp>
#include <iostream>

Camera::Camera()
	:m_pos(0, 0, 0), m_right(1, 0, 0), m_up(0, 1, 0), m_look(0, 0, 1)
{
}

void Camera::setPerspective(float fovY, float aspect, float zn, float zf)
{
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = zn;
	m_farZ = zf;

	m_nearWindowHeight = static_cast<float>(2.0f * m_nearZ * tan(0.5f*fovY));
	m_farWindowHeight = static_cast<float>(2.0f * m_farZ * tan(0.5f*fovY));

	m_proj = glm::perspective(fovY, aspect, m_nearZ, m_farZ);
}

void Camera::lookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& worldUp)
{
	m_look = glm::normalize(pos - target);

	m_right = glm::normalize(glm::cross(worldUp, m_look));

	m_up = glm::cross(m_look, m_right);

	glm::mat4 test1 = m_view;
	glm::mat4 test2 = view();
}

void Camera::strafe(float d)
{
	m_pos += m_right * d;
}

void Camera::walk(float d)
{
	m_pos += m_look * d;
}

void Camera::pitch(float angle)
{
	// Rotate up and look vector about the right vector.
	glm::mat4 rotation = glm::rotate(angle, m_right);

	m_look = glm::vec3(rotation * glm::vec4(m_look, 0));
	m_up = glm::vec3(rotation * glm::vec4(m_up, 0));
}

void Camera::rotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.
	m_right = glm::rotateY(m_right, angle);
	m_up = glm::rotateY(m_up, angle);
	m_look = glm::rotateY(m_look, angle);
}

void Camera::roll(float angle)
{
	// Rotate up and right vector about the look vector.
	glm::mat4 rotation = glm::rotate(angle, m_look);

	m_right = glm::vec3(rotation * glm::vec4(m_right, 0));
	m_up = glm::vec3(rotation * glm::vec4(m_up, 0));
}

void Camera::updateViewMatrix()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	m_look = normalize(m_look);
	m_up = normalize(cross(m_look, m_right));
	m_right = cross(m_up, m_look);

	// Fill in the view matrix entries.
	float x = -glm::dot(m_pos, m_right);
	float y = -glm::dot(m_pos, m_up);
	float z = -glm::dot(m_pos, m_look);

	m_view[0][0] = m_right.x;
	m_view[1][0] = m_right.y;
	m_view[2][0] = m_right.z;
	m_view[3][0] = x;
	
	m_view[0][1] = m_up.x;
	m_view[1][1] = m_up.y;
	m_view[2][1] = m_up.z;
	m_view[3][1] = y;
	
	m_view[0][2] = m_look.x;
	m_view[1][2] = m_look.y;
	m_view[2][2] = m_look.z;
	m_view[3][2] = z;
	
	m_view[0][3] = 0.0f;
	m_view[1][3] = 0.0f;
	m_view[2][3] = 0.0f;
	m_view[3][3] = 1.0f;
}