#pragma once
#include <Box2D/Dynamics/b2WorldCallbacks.h>

class ContactListener : public b2ContactListener
{
public:     
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
	void createCollisionToBomb(b2Body* sensor, b2Body* notSensor);
	void kickBomb(b2Body* sensor, b2Body* notSensor);
};
