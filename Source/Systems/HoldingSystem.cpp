#include "HoldingSystem.h"
#include "../GameGlobals.h"
#include "../Components/InventoryComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/BodyComponent.h"
#include "../Components/JumpComponent.h"
#include "../EntityFactory.h"
#include "../Components/CellComponent.h"
#include "JumpSystem.h"
#include "../Components/OwnerComponent.h"
#include "../Events/JumpEvent.h"

using namespace entityx;

HoldingSystem::~HoldingSystem()
{
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
	Entity bomb = holdEvent.bomb;
	auto inventory = holder.component<InventoryComponent>();
	
	if (bomb.has_component<BombComponent>() && !bomb.has_component<JumpComponent>())
	{
		if (bomb.has_component<BodyComponent>() && !(bomb.component<BodyComponent>()->body->GetLinearVelocity() == b2Vec2_zero)) return; //Bombe wurde gekickt, wer das dann noch halten kann ist Master ^^

		if (!inventory->isHoldingBomb)
		{
			if (bomb.has_component<BombComponent>())
			{
				inventory->holdingBombType = bomb.component<BombComponent>()->type;
			}

			assert(bomb.has_component<OwnerComponent>());
			inventory->holdingBombOwner = bomb.component<OwnerComponent>()->entity;
			inventory->isHoldingBomb = true;
			bomb.destroy();
			
			GameGlobals::events->emit<HoldingStatusEvent>(holder, true);
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
		Entity bomb = GameGlobals::entityFactory->createBomb(cell->x, cell->y, inventory->holdingBombOwner, inventory->holdingBombType);
		int x = 0, y = 0;
		JumpSystem::adjustXY_RelatingToTheDirection(&x, &y, 
													GameConstants::PUNCH_DISTANCE,
													whoThrows.component<DirectionComponent>()->direction);

		bomb.assign<JumpComponent>( whoThrows.component<DirectionComponent>()->direction, 
									cell->x, cell->y,
									cell->x+x, cell->y+y, 
									1.f, float(GameConstants::PUNCH_JUMPING_HEIGHT), float(GameConstants::PUNCH_JUMPING_SPEED));
		GameGlobals::events->emit<JumpEvent>(bomb);
		inventory->isHoldingBomb = false;
		GameGlobals::events->emit<HoldingStatusEvent>(whoThrows, false);
	}
}