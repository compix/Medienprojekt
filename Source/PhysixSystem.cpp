#include "PhysixSystem.h"

PhysixSystem::PhysixSystem(int velocityIterations, int positionIteration, float scale) :
m_World(b2Vec2(0,0)), m_Scale(scale), m_InvScale(1.f / m_Scale), m_velocityIterations(velocityIterations), m_positionIteration(positionIteration)
{
}

PhysixSystem::~PhysixSystem()
{
	delete &m_World;
}

b2World* PhysixSystem::GetWorld()
{
	return &m_World;
}

void PhysixSystem::SetWorld(b2World b2_world)
{
	m_World = b2_world;
}

void PhysixSystem::SetScale(float scale)
{
	m_Scale = scale;
	m_InvScale = 1.f / m_Scale;
}

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
	m_World.Step(dt,m_velocityIterations,m_positionIteration);
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
	return m_Scale;
}