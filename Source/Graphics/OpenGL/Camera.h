#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	void setPerspective(float fovY, float aspect, float zn, float zf);

	void lookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& worldUp);

	inline glm::mat4 viewProj() { return m_proj * m_view; }

	void strafe(float d);
	void walk(float d);
	void pitch(float angle);
	void rotateY(float angle);
	void roll(float angle);

	void updateViewMatrix();

	inline const glm::vec3& getPosition() const { return m_pos; }
	inline void setPosition(const glm::vec3& position) { m_pos = position; }

	inline const glm::vec3& getRight() const { return m_right; }
	inline const glm::vec3& getUp() const { return m_up; }
	inline const glm::vec3& getLook() const { return m_look; }
	inline const glm::mat4& view() const /* const - results in strange behavior in debug build */ { return m_view; }
	inline const glm::mat4& proj() const { return m_proj; }

	inline float getNearWindowWidth() const { return m_aspect * m_nearWindowHeight; }
	inline float getFarWindowWidth() const { return m_aspect * m_farWindowHeight; }
protected:
	// Camera coordinate system with coordinates relative to world space.
	glm::vec3 m_pos;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_look;

	// Cache frustum properties.
	float m_nearZ;
	float m_farZ;
	float m_aspect;
	float m_fovY;
	float m_nearWindowHeight;
	float m_farWindowHeight;

	// Cache View/Proj matrices.
public:
	glm::mat4 m_view;
	glm::mat4 m_proj;
};