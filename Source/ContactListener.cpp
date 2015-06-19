#include "ContactListener.h"
#include <iostream>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "../../Source/BodyFactory.h"
#include <entityx/Entity.h>
#include "Components/ItemComponent.h"
#include "Components/DirectionComponent.h"
#include "Components/InventoryComponent.h"

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
			std::cout << "CONTACT: "<< entity->id() << std::endl;
			if (entity->has_component<InventoryComponent>())
			{
				std::cout << "Has Inventory. " << std::endl;
				b2Vec2 force(0.f,0.f);
				float forcePower = 1.f;
				switch (entity->component<DirectionComponent>()->direction)
				{
				case Direction::UP:		force.y = -forcePower;	break;
				case Direction::DOWN:	force.y = forcePower;	break;
				case Direction::LEFT:	force.x = -forcePower;	break;
				case Direction::RIGHT:	force.x = forcePower;	break;
				default: break;
				}
				if (entity->component<InventoryComponent>()->bombKick)
				{
					sensor->SetLinearVelocity(force);
				}
				
			} 
		}
	}
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
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
	}
	else{

		if (ShouldCollide(fixtureA, fixtureB)){
			if (isA(fixtureA, BodyFactory::BOMB) && isA(fixtureB, ~BodyFactory::NOTHING))
			{

			}
		}

		if (ShouldCollide(fixtureA, fixtureB)){
			if (isA(fixtureA, BodyFactory::BOMB) && isA(fixtureB, BodyFactory::PLAYER)){
				kickBomb(fixtureA->GetBody(), fixtureB->GetBody());
			}
			else if (isA(fixtureB, BodyFactory::BOMB) && isA(fixtureA, BodyFactory::PLAYER))
			{
				kickBomb(fixtureB->GetBody(), fixtureA->GetBody());
			}
		}

	}
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
}

bool ContactListener::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	return ((fixtureA->GetFilterData().categoryBits & fixtureB->GetFilterData().maskBits) != 0);
}

bool ContactListener::isA(b2Fixture* fixture, uint16 category)
{
	return ((fixture->GetFilterData().categoryBits & category) !=0);
}