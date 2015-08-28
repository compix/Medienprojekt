#include "PhysixSystem.h"

float PhysixSystem::m_Scale(1.f);
float PhysixSystem::m_InvScale(1.f / PhysixSystem::m_Scale);

PhysixSystem::PhysixSystem(int velocityIterations, int positionIteration, float scale) :
m_velocityIterations(velocityIterations), m_positionIteration(positionIteration), m_World(b2Vec2(0,0))
{
	PhysixSystem::m_Scale = scale;
	PhysixSystem::m_InvScale = 1.f / PhysixSystem::m_Scale;
}

PhysixSystem::~PhysixSystem()
{
	//delete m_World;
}

b2World* PhysixSystem::GetWorld()
{
	return &m_World;
}

void PhysixSystem::SetWorld(b2World b2_world)
{
	m_World = b2_world;
}

/*void PhysixSystem::SetScale(float scale)
{
	PhysixSystem::m_Scale = scale;
	PhysixSystem::m_InvScale = 1.f / PhysixSystem::m_Scale;
}*/

void PhysixSystem::SetDebugDrawer(SFMLDebugDraw* debugDraw)
{
	m_World.SetDebugDraw(debugDraw);
}

void PhysixSystem::SetGravity(b2Vec2 gravity)
{
	m_World.SetGravity(gravity);
}

void PhysixSystem::Update(entityx::TimeDelta dt)
{
	m_World.Step(float32(dt), m_velocityIterations, m_positionIteration);
}

void PhysixSystem::DrawDebug()
{
	m_World.DrawDebugData();
}

b2Vec2 PhysixSystem::GetGravity()
{
	return m_World.GetGravity();
}

float PhysixSystem::GetScale()
{
	return PhysixSystem::m_Scale;
}

void PhysixSystem::setContactListener(b2ContactListener* listener)
{
	m_World.SetContactListener(listener);
}

b2Vec2 PhysixSystem::toBox2D(float px, float py)
{
	return b2Vec2(px / m_Scale, py / m_Scale);
}

float PhysixSystem::toBox2D(float pixel)
{
	return pixel / PhysixSystem::m_Scale;
}

float PhysixSystem::toWorld(float meters)
{
	return meters*PhysixSystem::m_Scale;
}
