#include "ContactListener.h"
#include <iostream>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "../../Source/BodyFactory.h"
#include <entityx/Entity.h>
#include "Components/ItemComponent.h"
#include "Components/DirectionComponent.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	#ifdef _DEBUG 
		//std::cout << "CONTACT" << std::endl;
	#endif

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	b2Body* sensor;
	b2Body* notSensor;

	//make sure only one of the fixtures was a sensor
	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();
	if ((sensorA ^ sensorB))
	{
		if (sensorA) { //fixtureB is not a Sensor
			sensor = fixtureA->GetBody();
			notSensor = fixtureB->GetBody();
		}
		else { //fixtureA is not a Sensor
			sensor = fixtureB->GetBody();
			notSensor = fixtureA->GetBody();
		}

		kickBomb(sensor, notSensor);

	}

	
}

void ContactListener::EndContact(b2Contact* contact)
{
	#ifdef _DEBUG 
	 //std::cout << "END_CONTACT" << std::endl;
	#endif

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	b2Body* sensor;
	b2Body* notSensor;

	//make sure only one of the fixtures was a sensor
	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();
	if ((sensorA ^ sensorB))
	{
		if (sensorA) { //fixtureB is not a Sensor
			sensor = fixtureA->GetBody();
			notSensor = fixtureB->GetBody();
		}
		else { //fixtureA is not a Sensor
			sensor = fixtureB->GetBody();
			notSensor = fixtureA->GetBody();
	}
	
	createCollisionToBomb(sensor, notSensor);

	}

	
}

void ContactListener::createCollisionToBomb(b2Body* sensor, b2Body* notSensor)
{
	auto fixture = sensor->GetFixtureList();
	while (fixture != nullptr)
	{
		auto filter = fixture->GetFilterData();
		filter.maskBits = ~BodyFactory::NOTHING;
		fixture->SetFilterData(filter);
		fixture = fixture->GetNext();
	}
}

void ContactListener::kickBomb(b2Body* sensor, b2Body* notSensor)
{
#ifdef _DEBUG 
	std::cout << "Kick entered." << std::endl;
#endif
	bool canKick = false;
	auto fixture = sensor->GetFixtureList();
	while (fixture != nullptr)
	{
		auto filter = fixture->GetFilterData();
		if (filter.maskBits != ~notSensor->GetFixtureList()->GetFilterData().categoryBits)
		{
			canKick = true;
		}
		fixture = fixture->GetNext();
	}

	if (canKick)
	{
		Entity* entity = static_cast<Entity*>(notSensor->GetUserData());

		if (entity != nullptr)
		{
			std::cout << "CONTACT: "<<entity->id() << std::endl;
			//entity->component<ItemComponent>();
		}
	}
	
	//entity->has_component<ItemComponent>();
	/*if ( && entity->component<ItemComponent>()->type == ItemType::BOMB_KICK_SKILL){

		b2Vec2 force;
		float forcePower = 10.f;
		switch (entity->component<DirectionComponent>()->direction)
		{
		case Direction::UP:		force.y = -forcePower;	break;
		case Direction::DOWN:	force.y = forcePower;	break;
		case Direction::LEFT:	force.x = -forcePower;	break;
		case Direction::RIGHT:	force.x = forcePower;	break;
		default: break;
		}
		notSensor->ApplyLinearImpulse(force, notSensor->GetLocalCenter(), true);
	}*/

}