#pragma once
#include <Box2D/Dynamics/b2WorldCallbacks.h>

class ContactListener : public b2ContactListener
{
public:
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
public:     
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
	void createCollisionToBomb(b2Body* sensor, b2Body* notSensor);
	void kickBomb(b2Body* sensor, b2Body* notSensor);

	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
	bool isA(b2Fixture* fixture, uint16 category);
};
