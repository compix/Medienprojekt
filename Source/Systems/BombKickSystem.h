#pragma once
#include <entityx/System.h>
#include <entityx/Entity.h>
#include "../LayerManager.h"
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include "../Components/CellComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/DirectionComponent.h"

class BombKickSystem : public entityx::System<BombKickSystem>
{

public:
	BombKickSystem(LayerManager* layerManager);
	
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;
	bool hasAntiMagnet(Entity e);
	void kickBomb(b2Body* sensor, b2Body* notSensor, Direction direction);
	bool fitIntoCell(SpriteComponent* spriteComponent, TransformComponent* transformComponent, CellComponent* cellComponent);
	void checkCollisionWithBomb(Entity e, Direction direction);
	b2Vec2 fitEntityIntoCell(CellComponent* cellComponent);
private:
	LayerManager* m_layerManager;
};

