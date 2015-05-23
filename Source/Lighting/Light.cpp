#include "Light.h"
#include <BodyFactory.h>
#include <PhysixSystem.h>
#include <Utils/Common.h>

const float EPSILON = 0.000001f;

float32 Light::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	// ignore fractions close to 0
	if ((fraction) <= EPSILON)
		return fraction;

	if ((fraction) < m_lowestFraction)
	{
		m_lowestFraction = fraction;
	}

	return fraction;
}

Light::Light()
	:m_shader(nullptr)
{
	
}

Light::Light(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation)
	: Light()
{
	create(center, color, radius, degree, rotation);
}

void Light::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_shader)
	{
		m_shader->setParameter("lightColor", m_color);
		m_shader->setParameter("lightPos", m_center);
		m_shader->setParameter("radius", m_radius);
		m_shader->setParameter("attenuation", m_attenuation);

		states.shader = m_shader;
	}

	states.blendMode = sf::BlendAdd;

	target.draw(m_vertices, states);
}

void Light::create(sf::Vector2f center, sf::Color color, float radius, float degree, float rotation)
{
	m_center = center;
	m_color = color;
	m_radius = radius;

	m_vertices.clear();
	m_vertices.setPrimitiveType(sf::TrianglesFan);
	//m_vertices.resize(SUBDIVISIONS + 1);

	m_vertices.append(sf::Vertex(center, color));

	float startDegree = rotation - degree / 2.f - Math::PI / 2.f;
	float stepSize = (Math::PI * 2.f) / SUBDIVISIONS;

	b2Vec2 start(PhysixSystem::toBox2D(center.x), PhysixSystem::toBox2D(center.y));

	b2RayCastOutput out;

	for (float angle = 0; angle <= std::min(Math::PI * 2.f, degree); angle += stepSize) 
	{
		m_lowestFraction = 1.f;

		m_nextVertex = sf::Vector2f(radius*cosf(angle + startDegree) + center.x, radius*sinf(angle + startDegree) + center.y);

		b2Vec2 end(PhysixSystem::toBox2D(m_nextVertex.x), PhysixSystem::toBox2D(m_nextVertex.y));
		BodyFactory::m_World->RayCast(this, start, end);

		m_nextVertex.x = m_center.x + (m_lowestFraction+0.005f) * (m_nextVertex.x - m_center.x);
		m_nextVertex.y = m_center.y + (m_lowestFraction+0.005f) * (m_nextVertex.y - m_center.y);

		sf::Vertex v(m_nextVertex);
		m_vertices.append(v);
	}

	m_attenuation = sf::Vector3f(radius*0.5f, 0.f, 1.f);
}