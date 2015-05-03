#include"PhysixSystem.h"

#ifndef _MSC_VER

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

PhysixSystem::PhysixSystem(float scale, int velocityIterations, int positionIterations){
	m_scale = make_unique<float>(scale);
	m_velocityIterations = velocityIterations;
	m_positionIterations = positionIterations;
	m_scaleInv = 1.0f / scale;
	m_world = new World(gravity, true);
	
}

PhysixSystem::~PhysixSystem(){
}

float PhysixSystem::GetScale() {
	return m_scale;
}

void PhysixSystem::Reset() {
	if (!m_world.IsLocked()) {
		b2Body* bodyList = m_world->GetBodyList();
		for (int i = 0; i < m_world->GetBodyCount(); i++) {
			m_world->DestroyBody(&bodyList[i]);
		}

		b2Joint* jointList = m_world.GetJointList();
		for (int i = 0; i < m_world->GetJointCount(); i++) {
			m_world->DestroyJoint(&jointList[i]);
		}
	}
}

b2World PhysixSystem::GetWorld() {
	return m_world;
}

void PhysixSystem::Destroy(b2Body* body) {
	m_world->DestroyBody(body);
}

void PhysixSystem::SetGravity(float x, float y) {
	m_gravity->Set(x, y);
	m_world->SetGravity(m_gravity);
	b2Body* bodies = m_world->GetBodyList();
	for (int i = 0; i < m_world->GetBodyCount(); i++) {
		bodies[i].SetAwake(true);

	}
}

void PhysixSystem::RopeConnect(b2Body* a, b2Body* b, float length) {
	b2RopeJointDef* ropeJointDef = new b2RopeJointDef();
	ropeJointDef->bodyA = a;
	ropeJointDef->bodyB = b;
	ropeJointDef->maxLength = length * m_scale;
	ropeJointDef->collideConnected = true;
	m_world->CreateJoint(ropeJointDef);
}

/**
* Convert world to box2d coordinates
*/
float PhysixSystem::ToBox2D(float pixel) {
	return pixel * m_scaleInv;
}

/**
* Convert box2d to world coordinates
*/
float PhysixSystem::ToWorld(float num) {
	return num * m_scale;
}

/**
* Convert world to box2d coordinates
*/
b2Vec2 PhysixSystem::ToBox2D(float x, float y, b2Vec2* out) {
	out->Set(x * m_scaleInv, y * m_scaleInv);
	return *out;
}

/**
* Convert world to box2d coordinates
*/
b2Vec2 PhysixSystem::ToBox2D(const b2Vec2 in, b2Vec2* out) {
	out->Set(in.x * m_scaleInv, in.y * m_scaleInv);
	return *out;
}

/**
* Convert box2d to world coordinates
*/
b2Vec2 PhysixSystem::ToWorld(const b2Vec2 in, b2Vec2* out) {
	out->Set(in.x * m_scale, in.y * m_scale);
	return *out;
}

void PhysixSystem::update(float deltaTime) {
	m_world->Step(deltaTime, m_velocityIterations, m_positionIterations);
	m_world->ClearForces();
}
