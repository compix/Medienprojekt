#pragma once
#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

const int SUBDIVISIONS = 360;
const float PI = 3.14159265359f;

class Light : public sf::Drawable, public b2RayCastCallback
{
public:
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override;

	Light();
	Light(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation);

	void create(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation);

	inline void setShader(sf::Shader* shader) { m_shader = shader; }

	inline void setAttenuation(sf::Vector3f attenuation) { m_attenuation = attenuation; }

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Shader* m_shader;

	sf::VertexArray m_vertices;

	sf::Vector2f m_center;
	sf::Color m_color;
	float m_radius;
	sf::Vector3f m_attenuation;

	sf::Vector2f m_nextVertex;
	float32 m_lowestFraction;
};