#include "BombKickSystem.h"
#include "../Components/LayerComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/BodyComponent.h"
#include "../Components/DirectionComponent.h"
#include "../BodyFactory.h"
#include "../GameConstants.h"
#include "../Components/SpriteComponent.h"
#include "../PhysixSystem.h"
#include <iostream>

BombKickSystem::BombKickSystem(LayerManager* layerManager)
	:m_layerManager(layerManager) {}


void BombKickSystem::addedToEngine(Engine *engine) {
	m_inputEntities = engine->getEntitiesFor(Family::all<InputComponent, CellComponent, LayerComponent>().get());
	m_bombEntities = engine->getEntitiesFor(Family::all<BombComponent, CellComponent, LayerComponent, BodyComponent>().get());
}

void BombKickSystem::update(float dt)
{
	//Check to kick Bombs
	for (Entity *player : *m_inputEntities)
	{
		auto cellComponent = player->get<CellComponent>();
		auto layerComponent = player->get<LayerComponent>();

		auto leftBombEntity = m_layerManager->getEntityWithComponent<BombComponent>(layerComponent->layer, cellComponent->x-1, cellComponent->y);
		auto downBombEntity = m_layerManager->getEntityWithComponent<BombComponent>(layerComponent->layer, cellComponent->x, cellComponent->y+1);
		auto rightBombEntity = m_layerManager->getEntityWithComponent<BombComponent>(layerComponent->layer, cellComponent->x+1, cellComponent->y);
		auto upBombEntity = m_layerManager->getEntityWithComponent<BombComponent>(layerComponent->layer, cellComponent->x, cellComponent->y-1);
		
		checkCollisionWithBomb(leftBombEntity,	Direction::LEFT);
		checkCollisionWithBomb(downBombEntity,	Direction::DOWN);
		checkCollisionWithBomb(rightBombEntity, Direction::RIGHT);
		checkCollisionWithBomb(upBombEntity,	Direction::UP);
		
	}

	//Check to stop kicked Bombs
	for (Entity *bombs : *m_bombEntities)
	{
		auto cellComponent = bombs->get<CellComponent>();
		auto layerComponent = bombs->get<LayerComponent>();

		auto leftWall = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cellComponent->x - 1, cellComponent->y);
		auto downWall = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cellComponent->x, cellComponent->y + 1);
		auto rightWall = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cellComponent->x + 1, cellComponent->y);
		auto upWall = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cellComponent->x, cellComponent->y - 1);

		b2Body* body = bombs->get<BodyComponent>()->body;
		b2Vec2 velo = body->GetLinearVelocity();

		if (velo.Length() != 0){ // Wenn Geschwindigkeits Vektorl�nge nicht null ist, also sich bewegt.
			if (velo.y < 0)
			{
				if (upWall){
					body->SetTransform(fitEntityIntoCell(cellComponent), 0);
					body->SetLinearVelocity(b2Vec2_zero);
				}
			}
			if (velo.y > 0)
			{
				if (downWall){
					body->SetTransform(fitEntityIntoCell(cellComponent), 0);
					body->SetLinearVelocity(b2Vec2_zero);
				}
			
			} 
			if (velo.x < 0)
			{
				if (leftWall){
					body->SetTransform(fitEntityIntoCell(cellComponent), 0);
					body->SetLinearVelocity(b2Vec2_zero);
				}
			}
			if (velo.x > 0)
			{
				if (rightWall){
					body->SetTransform(fitEntityIntoCell(cellComponent), 0);
					body->SetLinearVelocity(b2Vec2_zero);
				}
			}
		}
	}
}

void BombKickSystem::kickBomb(b2Body* sensor, b2Body* notSensor, Direction direction)
{
#ifdef _DEBUG 
	std::cout << "Kick entered." << std::endl;
#endif
	bool canKick = false;
	auto fixture = sensor->GetFixtureList();
	while (fixture != nullptr)
	{
		if (!fixture->IsSensor() && BodyFactory::ShouldCollide(fixture, notSensor->GetFixtureList()))
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
			if (entity->has<InventoryComponent>())
			{
				b2Vec2 force(0.f, 0.f);
				float forcePower = GameConstants::KICK_FORCE;
				switch (direction)
				{
				case Direction::UP:		force.y = -forcePower;	break;
				case Direction::DOWN:	force.y = forcePower;	break;
				case Direction::LEFT:	force.x = -forcePower;	break;
				case Direction::RIGHT:	force.x = forcePower;	break;
				default: break;
				}
				if (entity->get<InventoryComponent>()->bombKick && entity->get<DirectionComponent>()->direction == direction) //Wenn das Item aufgenommen wurde und die Richtung des Players, mit der Position der Bombe vom Spieler aus �bereinstimmt.
				{
					sensor->SetLinearVelocity(force);
				}
			}
		}
	}
}

bool BombKickSystem::fitIntoCell(SpriteComponent* spriteComponent, TransformComponent* transformComponent, CellComponent* cellComponent)
{
	if (transformComponent->x + spriteComponent->sprite.getLocalBounds().width <= (float)GameConstants::CELL_WIDTH * (cellComponent->x + 1)
		&& transformComponent->x - spriteComponent->sprite.getLocalBounds().width >= (float)GameConstants::CELL_WIDTH * (cellComponent->x) &&
		transformComponent->y + spriteComponent->sprite.getLocalBounds().height <= (float)GameConstants::CELL_HEIGHT * (cellComponent->y + 1)
		&& transformComponent->y - spriteComponent->sprite.getLocalBounds().height >= (float)GameConstants::CELL_HEIGHT * (cellComponent->y))
	{
		return true;
	}
	return false;
}

void BombKickSystem::checkCollisionWithBomb(Entity *e, Direction direction)
{
	if (e && e->get<BodyComponent>()->body->GetLinearVelocity().Length() == 0){
		for (b2ContactEdge* edge = e->get<BodyComponent>()->body->GetContactList(); edge; edge = edge->next)
		{
			b2Contact* contact = edge->contact;
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
					sensor = fixtureB->GetBody();
					notSensor = fixtureA->GetBody();
				}
			}

			if (BodyFactory::contactBetween(contact, BodyFactory::BOMB, BodyFactory::PLAYER)){
				if (BodyFactory::isA(fixtureA, BodyFactory::BOMB)){
					kickBomb(fixtureA->GetBody(), fixtureB->GetBody(), direction);
				}
				else
				{
					kickBomb(fixtureB->GetBody(), fixtureA->GetBody(), direction);
				}
			}
		}
	}
}

b2Vec2 BombKickSystem::fitEntityIntoCell(CellComponent* cellComponent)
{
	return b2Vec2(PhysixSystem::toBox2D((cellComponent->x + 1)*((float)GameConstants::CELL_WIDTH) - (float)GameConstants::CELL_WIDTH / 2), PhysixSystem::toBox2D((cellComponent->y + 1)*((float)GameConstants::CELL_HEIGHT) - (float)GameConstants::CELL_HEIGHT / 2));
}