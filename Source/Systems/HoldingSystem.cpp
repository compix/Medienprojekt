#include "HoldingSystem.h"
#include "../GameGlobals.h"
#include "../Components/InventoryComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/BodyComponent.h"
#include "../Components/JumpComponent.h"
#include "../EntityFactory.h"
#include "../Components/CellComponent.h"
#include "JumpSystem.h"

using namespace entityx;

HoldingSystem::~HoldingSystem()
{
	GameGlobals::events->unsubscribe<HoldingEvent>(*this);
}

void HoldingSystem::configure(entityx::EventManager& events)
{
	events.subscribe<HoldingEvent>(*this);
	events.subscribe<ThrowBombEvent>(*this);
}

void HoldingSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
}

void HoldingSystem::receive(const HoldingEvent& holdEvent)
{
	assert(holdEvent.whoHolds.has_component<InventoryComponent>());
	Entity holder = holdEvent.whoHolds;
	Entity wantToHold = holdEvent.wantToHold;
	auto inventory = holder.component<InventoryComponent>();
	
	if (wantToHold.has_component<BombComponent>() && !wantToHold.has_component<JumpComponent>())
	{
		if (wantToHold.has_component<BodyComponent>() && !(wantToHold.component<BodyComponent>()->body->GetLinearVelocity() == b2Vec2_zero)) return; //Bombe wurde gekickt, wer das dann noch halten kann ist Master ^^

		if (!inventory->isHoldingBomb)
		{
			wantToHold.destroy();
			inventory->isHoldingBomb = true;
		}
	}
}

void HoldingSystem::receive(const ThrowBombEvent& throwEvent)
{
	
	Entity whoThrows = throwEvent.whoThrows;
	assert(whoThrows.has_component<InventoryComponent>() && whoThrows.has_component<CellComponent>() && whoThrows.has_component<DirectionComponent>());
	auto inventory = whoThrows.component<InventoryComponent>();

	if (inventory->isHoldingBomb)
	{
		auto cell = whoThrows.component<CellComponent>();
		Entity bomb = GameGlobals::entityFactory->createBomb(cell->x, cell->y, whoThrows, inventory->isGhostBombActive(), inventory->isLightningBombActive());
		int x = 0, y = 0;
		JumpSystem::adjustXY_RelatingToTheDirection(&x, &y, 
													GameConstants::PUNCH_DISTANCE,
													whoThrows.component<DirectionComponent>()->direction);

		bomb.assign<JumpComponent>( whoThrows.component<DirectionComponent>()->direction, 
									cell->x, cell->y,
									cell->x+x, cell->y+y, 
									1, GameConstants::PUNCH_JUMPING_HEIGHT, GameConstants::PUNCH_JUMPING_SPEED);
		inventory->isHoldingBomb = false;
	}
}