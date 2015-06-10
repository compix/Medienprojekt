#include "ContactListener.h"
#include <iostream>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "../../Source/BodyFactory.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	#ifdef _DEBUG 
		std::cout << "CONTACT" << std::endl;
	#endif
	

	
}

void ContactListener::EndContact(b2Contact* contact)
{
	#ifdef _DEBUG 
	 std::cout << "END_CONTACT" << std::endl;
	#endif

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	//make sure only one of the fixtures was a sensor
	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();
	if ((sensorA ^ sensorB))
	{
		auto bodyA = fixtureA->GetBody();
		auto bodyB = fixtureB->GetBody();

		if (sensorA) { //fixtureB is not a Sensor
			auto fixture = bodyA->GetFixtureList();
			while (fixture != nullptr)
			{
				auto filter = fixture->GetFilterData();
				filter.maskBits = ~BodyFactory::NOTHING;
				fixture->SetFilterData(filter);
				fixture = fixture->GetNext();
			}

		}
		else { //fixtureA is not a Sensor
			;
			auto fixture = bodyB->GetFixtureList();
			while (fixture != nullptr)
			{
				auto filter = fixture->GetFilterData();
				filter.maskBits = ~BodyFactory::NOTHING;
				fixture->SetFilterData(filter);
				fixture = fixture->GetNext();
			}
	}
	}
}