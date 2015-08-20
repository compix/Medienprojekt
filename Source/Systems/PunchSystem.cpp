#include "PunchSystem.h"
#include "../Components/InventoryComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/JumpComponent.h"
#include "../Components/LayerComponent.h"
#include "../BodyFactory.h";
#include "../Components/AIComponent.h"
#include "../PhysixSystem.h"
#include "../Components/TimerComponent.h"
#include "../Components/RenderOffsetComponent.h"

PunchSystem::PunchSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

PunchSystem::~PunchSystem()
{
}

void PunchSystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<PunchEvent>(*this);
}

void PunchSystem::receive(const PunchEvent& event)
{
	auto entity = event.triggerEntity;
	if (!entity.has_component<AIComponent>()){
		assert(entity && entity.has_component<InventoryComponent>() && entity.has_component<DirectionComponent>() && entity.has_component<CellComponent>());

		auto cellComponent = entity.component<CellComponent>();

		Entity bomb;

		int x = 0, y = 0;
		int jumpDistance = event.punchDistance;
		switch (entity.component<DirectionComponent>()->direction)
		{
		case Direction::UP:
			bomb = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x, cellComponent->y - 1);
			y = -jumpDistance;
			break;
		case Direction::DOWN:
			bomb = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x, cellComponent->y + 1);
			y = +jumpDistance;
			break;
		case Direction::LEFT:
			bomb = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x - 1, cellComponent->y);
			x = -jumpDistance;
			break;
		case Direction::RIGHT:
			bomb = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x + 1, cellComponent->y);
			x = +jumpDistance;
			break;
		default: break;
		}

		if (!bomb)
			return;

		auto cpBomb = bomb.component<CellComponent>();
		bomb.assign<JumpComponent>(entity.component<DirectionComponent>()->direction, cpBomb->x, cpBomb->y, cpBomb->x + x, cpBomb->y + y, 1,5,4);
	}
}

void PunchSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{

	for (auto jumpingEntity : entityManager.entities_with_components<JumpComponent, BodyComponent>())
	{
		auto jumpComp = jumpingEntity.component<JumpComponent>();
		auto body = jumpingEntity.component<BodyComponent>();

		deactivateCollisionForFlyingBodys(body);
		deactivateTimerForBombs(jumpingEntity, jumpComp);
		jumpFunction(jumpingEntity, jumpComp, body, dt);

		if (jumpComp->timePassed == jumpComp->totalTime) //Wenn am Ziel angekommen
		{
			activateTimerForBombs(jumpingEntity);
			activateCollisionForFlyingBodys(body);
			removeRenderOffset(jumpingEntity, jumpComp, body);
			jumpingEntity.component<LayerComponent>()->layer = GameConstants::MAIN_LAYER;
			jumpingEntity.remove<JumpComponent>();
		}
	}
}

void PunchSystem::jumpFunction(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp, ComponentHandle<BodyComponent, EntityManager> body, TimeDelta dt)
{
	float power = jumpComp->startVelocity, g = GameConstants::EARTH_GRAVITY;

	auto blockingTile = m_layerManager->getEntityWithComponent<BodyComponent>(GameConstants::MAIN_LAYER, jumpComp->toX, jumpComp->toY);

	if (blockingTile) //Reagier auf �nderungen des Ziel Tiles (Ob die Bombe etwas h�hwer landet oder nicht, wegen Blockierung)
	{
		if (jumpComp->targetIsBlocked != true)
		{
			jumpComp->targetIsBlocked = true;
			jumpComp->isDegreeCalculated = false;
		}
	} else
	{
		if (jumpComp->targetIsBlocked != false)
		{
			jumpComp->targetIsBlocked = false;
			jumpComp->isDegreeCalculated = false;
		}
	}

	if (!jumpComp->isDegreeCalculated)
	{
		jumpingEntity.component<LayerComponent>()->layer = GameConstants::JUMP_LAYER;

		float x = -((jumpComp->fromX * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f) - (jumpComp->toX * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f));
		float y = (jumpComp->fromY * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f) - (jumpComp->toY*GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / (jumpComp->targetIsBlocked ? 10.f:2.f));

		jumpComp->degreeX = x / (power*jumpComp->totalTime);
		jumpComp->degreeY = (y + g / 2 * powf(jumpComp->totalTime, 2)) / (power*jumpComp->totalTime);

		jumpComp->isDegreeCalculated = true;
	}

	jumpComp->timePassed += dt*jumpComp->deltaTimeMultiplikator;
	if (jumpComp->timePassed > jumpComp->totalTime)
	{
		jumpComp->timePassed = jumpComp->totalTime;
	}

	float x = power * jumpComp->degreeX * jumpComp->timePassed;
	float y = power * jumpComp->degreeY * jumpComp->timePassed - ((g / 2.f)*powf(jumpComp->timePassed, 2));


	if (!jumpingEntity.has_component<RenderOffsetComponent>()) //Ein versatz des Renderings, damit das eigentliche Objekt nicht aus der Map gelangt.
	{
		jumpingEntity.assign<RenderOffsetComponent>(x, -y);

		float finalX = power * jumpComp->degreeX * jumpComp->totalTime;
		float finalY = power * jumpComp->degreeY * jumpComp->totalTime - ((g / 2.f)*powf(jumpComp->totalTime, 2));

		body->body->SetTransform(b2Vec2(PhysixSystem::toBox2D(jumpComp->fromX * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f + finalX),
			PhysixSystem::toBox2D(jumpComp->fromY * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f - finalY)),
			0);

		if (jumpingEntity.has_component<TransformComponent>())
		{
			auto trans = jumpingEntity.component<TransformComponent>();
			trans->x = jumpComp->fromX * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f + finalX;
			trans->y = jumpComp->fromY * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f - finalY;
		}
	} else
	{
		jumpingEntity.component<RenderOffsetComponent>()->xOffset = x;
		jumpingEntity.component<RenderOffsetComponent>()->yOffset = -y;
	}
	

}

void PunchSystem::deactivateTimerForBombs(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp)
{
	if (!jumpComp->isDegreeCalculated)
	{
		if (jumpingEntity.has_component<BombComponent>() && jumpingEntity.has_component<TimerComponent>())
		{
			jumpingEntity.component<TimerComponent>()->active = false;
		}
	}
}

void PunchSystem::activateTimerForBombs(Entity jumpingEntity)
{
	if (jumpingEntity.has_component<BombComponent>() && jumpingEntity.has_component<TimerComponent>())
	{
		jumpingEntity.component<TimerComponent>()->active = true;
	}
}

void PunchSystem::deactivateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body)
{
	auto fixturList = body->body->GetFixtureList();

	while (fixturList != nullptr)
	{
		if (BodyFactory::isA(fixturList, BodyFactory::BOMB))
			fixturList->SetSensor(true);
		fixturList = fixturList->GetNext();
	}
}

void PunchSystem::activateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body)
{
	auto fixturList = body->body->GetFixtureList();

	while (fixturList != nullptr)
	{
		if (BodyFactory::isA(fixturList, BodyFactory::BOMB))
			fixturList->SetSensor(false);
		fixturList = fixturList->GetNext();
	}
}

void PunchSystem::removeRenderOffset(Entity jumping_entity, ComponentHandle<JumpComponent> jump_comp, ComponentHandle<BodyComponent> body)
{
	if (jumping_entity.has_component<RenderOffsetComponent>())
	{
		auto roc = jumping_entity.component<RenderOffsetComponent>();
		roc->remove = true;
	}
}