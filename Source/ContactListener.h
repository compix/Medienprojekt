#pragma once
#include <Box2D/Dynamics/b2WorldCallbacks.h>

class ContactListener : public b2ContactListener
{
public:     
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};
