#pragma once
#include <Box2D/Box2D.h>
#include <entityx/config.h>
#include "SFMLDebugDraw.h"

class PhysixSystem
{
	enum _entityCategory {
		PLAYER = 0x0001,
		WALL = 0x0002
	};

public:
	PhysixSystem(int velocityIterations, int positionIteration, float scale);
	~PhysixSystem();
	
	void SetWorld(b2World b2_world);
	void SetScale(float scale);
	void SetDebugDrawer(SFMLDebugDraw* debugDraw);
	void SetGravity(b2Vec2 gravity);

	void Update(entityx::TimeDelta dt);
	void DrawDebug();
	b2Vec2 GetGravity();
	b2World* GetWorld();
	float GetScale();
	
private:
	int m_velocityIterations, m_positionIteration;
	float m_Scale, m_InvScale;
	b2World m_World;
};
