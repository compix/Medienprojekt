#pragma once
#include <Box2D/Box2D.h>
#include <entityx/config.h>
#include "SFMLDebugDraw.h"

class PhysixSystem
{
	

public:
	PhysixSystem(int velocityIterations, int positionIteration, float scale);
	~PhysixSystem();

	static float m_Scale, m_InvScale;
	
	void SetWorld(b2World b2_world);
	//void SetScale(float scale);
	void SetDebugDrawer(SFMLDebugDraw* debugDraw);
	void SetGravity(b2Vec2 gravity);

	void Update(entityx::TimeDelta dt);
	void DrawDebug();
	b2Vec2 GetGravity();
	b2World* GetWorld();
	float GetScale();

	void setContactListener(b2ContactListener* listener);

	static float toBox2D(float pixel);
	static float toWorld(float meters);
	
private:
	int m_velocityIterations, m_positionIteration;
	b2World m_World;
};
