#include "ContactListener.h"
#include <iostream>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "../../Source/BodyFactory.h"
#include <entityx/Entity.h>
#include "Components/OwnerComponent.h"
#include "Components/BlinkComponent.h"
#include "Components/BodyComponent.h"
#include "Game.h"


void ContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	Entity entityA = GameGlobals::entities->get(GameGlobals::entities->create_id(reinterpret_cast<int>(fixtureA->GetBody()->GetUserData())));
	Entity entityB = GameGlobals::entities->get(GameGlobals::entities->create_id(reinterpret_cast<int>(fixtureB->GetBody()->GetUserData())));
	Entity withBlink;

	if (entityA.has_component<BlinkComponent>())
	{
		withBlink = entityA;
	}
	if (entityB.has_component<BlinkComponent>())
	{
		withBlink = entityB;
	}

	if (withBlink){
		withBlink.component<BodyComponent>()->body->SetLinearVelocity(b2Vec2_zero);
	}
}

void ContactListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	b2Body* sensor;
	b2Body* notSensor;

	//Sicherstellen. dass nur ein fixture ein Sensor ist
	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();
	if ((sensorA ^ sensorB))
	{
		if (sensorA) { //fixtureB ist kein Sensor
			sensor = fixtureA->GetBody();
			notSensor = fixtureB->GetBody();
		}
		else { //fixtureA ist kein Sensor
			auto temp = fixtureA;
			fixtureA = fixtureB;
			fixtureB = temp;

			sensor = fixtureA->GetBody();
			notSensor = fixtureB->GetBody();
	}
		Entity entityNotSensor = GameGlobals::entities->get(GameGlobals::entities->create_id(reinterpret_cast<int>(notSensor->GetUserData())));
		Entity entitySensor = GameGlobals::entities->get(GameGlobals::entities->create_id(reinterpret_cast<int>(sensor->GetUserData())));

		if (BodyFactory::contactBetween(contact,BodyFactory::BOMB_RADAR,BodyFactory::PLAYER)){
			if (entitySensor.component<OwnerComponent>()->entity.id() == entityNotSensor.id() && sensor->GetFixtureList()->GetNext()->GetFilterData().categoryBits == BodyFactory::BOMB){
				createCollisionToBomb(sensor, notSensor);
			}
		}
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

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}


