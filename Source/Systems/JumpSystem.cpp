#include "JumpSystem.h"
#include "../Components/InventoryComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/JumpComponent.h"
#include "../Components/LayerComponent.h"
#include "../BodyFactory.h"
#include "../Components/AIComponent.h"
#include "../PhysixSystem.h"
#include "../Components/TimerComponent.h"
#include "../Components/RenderOffsetComponent.h"
#include "../GameGlobals.h"
#include "../Game.h"
#include "../Components/PlayerComponent.h"
#include "../Events/SkillEvent.h"

JumpSystem::JumpSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

JumpSystem::~JumpSystem()
{
}

void JumpSystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<SkillEvent>(*this);
}

void JumpSystem::receive(const SkillEvent& event)
{
	if (event.type != SkillType::PUNCH)
		return;

	auto entity = event.triggerEntity;
	assert(entity && entity.has_component<InventoryComponent>() && entity.has_component<DirectionComponent>() && entity.has_component<CellComponent>());

	auto cellComponent = entity.component<CellComponent>();
	auto direction = entity.component<DirectionComponent>()->direction;

	Entity bomb;

	int x = 0, y = 0;
	int jumpDistance = GameConstants::PUNCH_DISTANCE;
	adjustXY_RelatingToTheDirection(&x, &y, jumpDistance, direction, cellComponent, &bomb);

	if (!bomb)
		return;

	if (bomb.has_component<BodyComponent>() && bomb.component<BodyComponent>()->body->GetLinearVelocity().Length() == 0) //Wenn Bombe nicht gekickt worden sind
	{
		auto cpBomb = bomb.component<CellComponent>();
		bomb.assign<JumpComponent>(direction, cpBomb->x, cpBomb->y, cpBomb->x + x, cpBomb->y + y, 1, GameConstants::PUNCH_JUMPING_HEIGHT, GameConstants::PUNCH_JUMPING_SPEED);
	}
}

void JumpSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	for (auto jumpingEntity : entityManager.entities_with_components<JumpComponent, CellComponent>())
	{
		auto jumpComp = jumpingEntity.component<JumpComponent>();
		auto cellComp = jumpingEntity.component<CellComponent>();

		ComponentHandle<BodyComponent> body;
		if (jumpingEntity.has_component<BodyComponent>())
		{
			 body = jumpingEntity.component<BodyComponent>();
			 deactivateCollisionForFlyingBodys(body);
		}
		
		

		
		deactivateTimerForBombs(jumpingEntity, jumpComp);
		jumpFunction(jumpingEntity, jumpComp, dt);

		

		if (jumpComp->timePassed == jumpComp->totalTime) //Wenn am Ziel angekommen
		{
			bool targetBlocked = jumpingEntity.component<JumpComponent>()->targetIsBlocked;
			Direction lastDirection = jumpingEntity.component<JumpComponent>()->direction;
			removeRenderOffset(jumpingEntity);
			float deltaX = getXCoords(static_cast<int>(getDeltaOf(static_cast<float>(jumpComp->toX), static_cast<float>(jumpComp->fromX))));
			float deltaY = getYCoords(static_cast<int>(getDeltaOf(static_cast<float>(jumpComp->toY), static_cast<float>(jumpComp->fromY))));

			jumpingEntity.remove<JumpComponent>();
			if (targetBlocked)
			{
				int x = 0, y = 0;
				int jumpDistance = 1;
				adjustXY_RelatingToTheDirection(&x, &y, jumpDistance, lastDirection);

				jumpingEntity.assign<JumpComponent>(lastDirection, cellComp->x, cellComp->y, cellComp->x + x, cellComp->y + y, 1, GameConstants::BLOCKED_CELL_JUMPING_HEIGHT, GameConstants::BLOCKED_CELL_JUMPING_SPEED);
				jumpingEntity.component<JumpComponent>()->wasBlocked = true;

				auto entityWithInventory = m_layerManager->getEntityWithComponent<InventoryComponent>(GameConstants::MAIN_LAYER, cellComp->x, cellComp->y);

				if (entityWithInventory && !jumpingEntity.has_component<ItemComponent>())
				{
					auto inv = entityWithInventory.component<InventoryComponent>();
					SkillType st = inv->activeSkill();
					inv->removeSkill(st);
					Entity item;
					switch (st){
					case SkillType::PLACE_PORTAL:
						item = GameGlobals::entityFactory->createItem(cellComp->x, cellComp->y, ItemType::PORTAL_SKILL);
						break;
					case SkillType::PUNCH:
						item = GameGlobals::entityFactory->createItem(cellComp->x, cellComp->y, ItemType::PUNCH_SKILL);
						break;
					case SkillType::BLINK:
						item = GameGlobals::entityFactory->createItem(cellComp->x, cellComp->y, ItemType::BLINK_SKILL);
						break;
					}
					if (item){
						item.component<LayerComponent>()->layer = GameConstants::JUMP_LAYER;
						uint8_t direction = (rand() % 4);

						int x = 0, y = 0;
						switch (static_cast<Direction>(direction))
						{
						case Direction::UP:
							y = -5;
							break;
						case Direction::DOWN:
							y = 5;
							break;
						case Direction::LEFT:
							x = -5;
							break;
						case Direction::RIGHT:
							x = 5;
							break;
						default: break;
						}

						item.assign<JumpComponent>(static_cast<Direction>(direction), cellComp->x, cellComp->y, cellComp->x + ((rand() % 6) - 5), cellComp->y + ((rand() % 6) - 5), 1, GameConstants::PUNCH_JUMPING_HEIGHT, GameConstants::PUNCH_JUMPING_SPEED);
					}
				}

				auto roc = jumpingEntity.component<RenderOffsetComponent>();
				float xOffset = roc->xOffset - deltaX;
				float yOffset = roc->yOffset -deltaY;

				jumpingEntity.remove<RenderOffsetComponent>();
				jumpingEntity.assign<RenderOffsetComponent>(xOffset, yOffset);
			}
			else { // Sachen die wiederhergestellt werden müssen, wenn wirklich am Ziel angekommen ist
				if (body){
					activateCollisionForFlyingBodys(body);
				}
				jumpingEntity.component<LayerComponent>()->layer = GameConstants::MAIN_LAYER;
				activateTimerForBombs(jumpingEntity);
			}

		}
	}
}

void JumpSystem::jumpFunction(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp, TimeDelta dt)
{

		int fromCheckX = jumpComp->fromX;
		int fromCheckY = jumpComp->fromY;
		int toCheckX = jumpComp->toX;
		int toCheckY = jumpComp->toY;
		adjustCellsIfOutOfBounds(&fromCheckX, &toCheckX, &fromCheckY, &toCheckY);

		EntityCollection entitiesOnTarget = m_layerManager->getEntities(GameConstants::MAIN_LAYER, toCheckX, toCheckY);
		bool targetBlocked = false;
		for (auto it = entitiesOnTarget.begin(); it != entitiesOnTarget.end(); ++it)
		{
			if (jumpingEntity.has_component<ItemComponent>())
			{
				if (it->id() != jumpingEntity.id() && it->has_component<BodyComponent>() && !it->has_component<PlayerComponent>())
				{
					targetBlocked = true;
				}
			}
			else
			if (it->id() != jumpingEntity.id() && it->has_component<BodyComponent>()){ //Wenn ein Hindernis außer die Bombe selbst es Blockiert
					targetBlocked = true;
				break;
			}
		}
	checkIfDegreeMustBeRecalculated(jumpComp, targetBlocked);

	float beginHeight = 0, endHeight = 0, offHeight = GameConstants::CELL_HEIGHT / 4.f;
	adjustHeightForBlockedTiles(jumpComp->wasBlocked, jumpComp->targetIsBlocked, &beginHeight, &endHeight, offHeight);
	

	if (!jumpComp->isDegreeCalculated)
	{
		jumpingEntity.component<LayerComponent>()->layer = GameConstants::JUMP_LAYER;
		calculateDegreeOfJumpComp(jumpComp, endHeight);
		jumpComp->isDegreeCalculated = true;
	}

	jumpComp->timePassed += dt*jumpComp->deltaTimeMultiplikator;
	if (jumpComp->timePassed > jumpComp->totalTime)
	{
		jumpComp->timePassed = jumpComp->totalTime;
	}


	float calculatedXPos, calculatedYPos;
	calculateXY_ForOffset(&calculatedXPos, &calculatedYPos, jumpComp, beginHeight);



	if (!jumpingEntity.has_component<RenderOffsetComponent>()) //Ein versatz des Renderings, damit das eigentliche Objekt nicht aus der Map gelangt.
	{
		jumpingEntity.assign<RenderOffsetComponent>(calculatedXPos, -calculatedYPos);
	} else
	{
		if (jumpComp->timePassed > jumpComp->totalTime/2.f)
		{
			//if (targetIsOutOfBounds(jumpComp->toX, jumpComp->toY)){
				adjustCellsIfOutOfBounds(&jumpComp->fromX, &jumpComp->toX, &jumpComp->fromY, &jumpComp->toY);
			//}

			setBodyAndTransformOfEntityToTarget(jumpingEntity);
		}

		jumpingEntity.component<RenderOffsetComponent>()->xOffset = calculatedXPos;
		jumpingEntity.component<RenderOffsetComponent>()->yOffset = -calculatedYPos;
	}
	

}

void JumpSystem::adjustXY_RelatingToTheDirection(int* x, int* y, int step, Direction direction, ComponentHandle<CellComponent> cellComponent, Entity* foundEntity)
{
	switch (direction)
	{
	case Direction::UP:
		*foundEntity = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x, cellComponent->y - 1);
		*y = -step;
		break;
	case Direction::DOWN:
		*foundEntity = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x, cellComponent->y + 1);
		*y = +step;
		break;
	case Direction::LEFT:
		*foundEntity = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x - 1, cellComponent->y);
		*x = -step;
		break;
	case Direction::RIGHT:
		*foundEntity = m_layerManager->getEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cellComponent->x + 1, cellComponent->y);
		*x = +step;
		break;
	default: break;
	}
}

void JumpSystem::adjustXY_RelatingToTheDirection(int* x, int* y, int step, Direction direction)
{
	switch (direction)
	{
	case Direction::UP:
		*y = -step;
		break;
	case Direction::DOWN:
		*y = +step;
		break;
	case Direction::LEFT:
		*x = -step;
		break;
	case Direction::RIGHT:
		*x = +step;
		break;
	default: break;
	}
}

void JumpSystem::adjustCellsIfOutOfBounds(int* fromX, int* toX, int* fromY, int* toY)
{
	assert(fromX != nullptr || fromY != nullptr || toY != nullptr || toX != nullptr);
	int deltaX = static_cast<int>(abs(getDeltaOf(static_cast<float>(*toX), static_cast<float>(*fromX))));
	int deltaY = static_cast<int>(abs(getDeltaOf(static_cast<float>(*toY), static_cast<float>(*fromY))));
	
	if (*toX >= GameGlobals::game->getWidth()){
		*toX = deltaX-1;
		*fromX = -1;
	}
	if (*toX < 0){
		*toX = GameGlobals::game->getWidth() - deltaX;
		*fromX = GameGlobals::game->getWidth();
	}
	if (*toY >= GameGlobals::game->getHeight()){
		*toY = deltaY - 1;
		*fromY = -1;
		
	}
	if (*toY < 0){
		*toY = GameGlobals::game->getHeight() - deltaY;
		*fromY = GameGlobals::game->getHeight();
	}
}

void JumpSystem::adjustHeightForBlockedTiles(const bool wasBlocked,const bool targetIsBlocked, float* beginHeight,float* endHeight,const float offHeight)
{
	if (!wasBlocked && targetIsBlocked)
	{
		*beginHeight = 0; *endHeight = offHeight;
	}

	if (wasBlocked && targetIsBlocked)
	{
		*beginHeight = offHeight; *endHeight = 0;
	}

	if (wasBlocked && !targetIsBlocked)
	{
		*beginHeight = offHeight; *endHeight = -offHeight;
	}
}

float JumpSystem::getDeltaOf(float coord2, float coord1)
{
	return coord2 - coord1;
}

float JumpSystem::getXCenterCoords(int cellX)
{
	return cellX*GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f;
}

float  JumpSystem::getYCenterCoords(int cellY)
{
	return cellY * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f;
}

float JumpSystem::getXCoords(int cellX)
{
	return static_cast<float>(cellX*GameConstants::CELL_WIDTH);
}

float  JumpSystem::getYCoords(int cellY)
{
	return static_cast<float>(cellY * GameConstants::CELL_HEIGHT);
}

void JumpSystem::calculateDegreeOfJumpComp(ComponentHandle<JumpComponent, EntityManager> jumpComp, float endHeight)
{
	float deltaX = getDeltaOf(getXCenterCoords(jumpComp->toX), getXCenterCoords(jumpComp->fromX));
	float deltaY = -getDeltaOf(((jumpComp->toY)*GameConstants::CELL_HEIGHT + (GameConstants::CELL_HEIGHT / 2.f) - endHeight), getYCenterCoords(jumpComp->fromY));

	jumpComp->degreeX = static_cast<float>(deltaX / (jumpComp->startVelocity*static_cast<float>(jumpComp->totalTime)));
	jumpComp->degreeY = static_cast<float>(((deltaY + GameConstants::EARTH_GRAVITY / 2 * powf(static_cast<float>(jumpComp->totalTime), 2))) / (jumpComp->startVelocity*static_cast<float>(jumpComp->totalTime)));
}

void JumpSystem::calculateXY_ForOffset(float* xPos, float* yPos, ComponentHandle<JumpComponent, EntityManager> jumpComp, float beginHeight)
{
	*xPos = static_cast<float>(jumpComp->startVelocity * jumpComp->degreeX * jumpComp->timePassed);
	*yPos = static_cast<float>(beginHeight + jumpComp->startVelocity * jumpComp->degreeY * jumpComp->timePassed - ((GameConstants::EARTH_GRAVITY / 2.f)*powf(jumpComp->timePassed, 2)));
}

void JumpSystem::checkIfDegreeMustBeRecalculated(ComponentHandle<JumpComponent, EntityManager> jumpComp, bool targetBlocked)
{
	if (targetBlocked) //Reagier auf änderungen des Ziel Tiles (Ob die Bombe etwas höher landet oder nicht, wegen Blockierung)
	{
		jumpComp->targetIsBlocked = true;
		if (jumpComp->targetIsBlocked != jumpComp->targetWasBlocked)
		{
			jumpComp->targetWasBlocked = jumpComp->targetIsBlocked;
			jumpComp->isDegreeCalculated = false;
		}
	}
	else
	{
		jumpComp->targetIsBlocked = false;
		if (jumpComp->targetIsBlocked != jumpComp->targetWasBlocked)
		{
			jumpComp->targetWasBlocked = jumpComp->targetIsBlocked;
			jumpComp->isDegreeCalculated = false;
		}
	}
}

void JumpSystem::setBodyAndTransformOfEntityToTarget(Entity jumpingEntity)
{
	auto jumpComp = jumpingEntity.component<JumpComponent>();

	if (jumpingEntity.has_component<BodyComponent>())
	{
		jumpingEntity.component<BodyComponent>()->body->SetTransform(b2Vec2(PhysixSystem::toBox2D(getXCenterCoords(jumpComp->toX)),
			PhysixSystem::toBox2D(getYCenterCoords(jumpComp->toY))),
			0);
	}
	if (jumpingEntity.has_component<TransformComponent>())
	{
		auto trans = jumpingEntity.component<TransformComponent>();
		trans->x = getXCenterCoords(jumpComp->toX);
		trans->y = getYCenterCoords(jumpComp->toY);
	}
}

void JumpSystem::deactivateTimerForBombs(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp)
{
	if (!jumpComp->isDegreeCalculated)
	{
		if (jumpingEntity.has_component<BombComponent>() && jumpingEntity.has_component<TimerComponent>())
		{
			jumpingEntity.component<TimerComponent>()->active = false;
		}
	}
}

void JumpSystem::activateTimerForBombs(Entity jumpingEntity)
{
	if (jumpingEntity.has_component<BombComponent>() && jumpingEntity.has_component<TimerComponent>())
	{
		jumpingEntity.component<TimerComponent>()->active = true;
	}
}

void JumpSystem::deactivateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body)
{
	auto fixturList = body->body->GetFixtureList();

	while (fixturList != nullptr)
	{
		if (BodyFactory::isA(fixturList, BodyFactory::BOMB))
			fixturList->SetSensor(true);
		fixturList = fixturList->GetNext();
	}
}

void JumpSystem::activateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body)
{
	auto fixturList = body->body->GetFixtureList();

	while (fixturList != nullptr)
	{
		if (BodyFactory::isA(fixturList, BodyFactory::BOMB))
			fixturList->SetSensor(false);
		fixturList = fixturList->GetNext();
	}
}

void JumpSystem::removeRenderOffset(Entity jumping_entity)
{
	if (jumping_entity.has_component<RenderOffsetComponent>())
	{
		auto roc = jumping_entity.component<RenderOffsetComponent>();
		roc->remove = true;
	}
}

bool JumpSystem::targetIsOutOfBounds(int toX, int toY)
{
	if (toX >= GameGlobals::game->getWidth() - 1){
		return true;
	}

	if (toX <= 0){
		return true;
	}

	if (toY >= GameGlobals::game->getHeight() - 1){
		return true;
	}

	if (toY <= 0){
		return true;
	}
	return false;
}