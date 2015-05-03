#pragma once
#include <Box2D/Box2D.h>
#include <memory>

class PhysixSystem
{
public:
	PhysixSystem(float scale, int velocityIterations, int positionIterations);
	~PhysixSystem();
	unique_ptr<float> m_scale;
	unique_ptr<float> m_scaleInv;

private:
	unique_ptr<b2World> m_world;
	unique_ptr<b2Vec2> m_gravity;
	unique_ptr<int> m_velocityIterations;
	unique_ptr<int> m_positionIterations;

public:
	float GetScale();
	void Reset();
	b2World GetWorld();
	void Destroy(b2Body* body);
	void SetGravity(float x, float y);
	void RopeConnect(b2Body* a, b2Body* b, float length);
	float ToBox2D(float pixel);
	float ToWorld(float num);
	b2Vec2 ToBox2D(float x, float y, b2Vec2* out);
	b2Vec2 ToBox2D(const b2Vec2 in, b2Vec2* out);
	b2Vec2 ToWorld(const b2Vec2 in, b2Vec2* out);
	void update(float deltaTime);
};