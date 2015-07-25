#include "AISystem.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Utils/PathFinding/PathEngine.h"
#include "../GameConstants.h"
#include "../Utils/Random.h"
#include "../AI/PlaceBomb.h"
#include "../Events/ItemPickedUpEvent.h"
#include "../Game.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BlockComponent.h"
#include "../AI/FollowPath.h"
#include "../AI/MoveTo.h"
#include "../AI/PathRatings/RateSafety.h"
#include "../AI/PathRatings/RateItem.h"
#include "../AI/PathRatings/RateDestroyBlockSpot.h"
#include "../AI/PathRatings/RateEscape.h"
#include "../AI/PathRatings/RateCombination.h"
#include "../AI/PathRatings/RateTrapDanger.h"
#include "../AI/Checks/IsSafePath.h"
#include "../AI/PathRatings/RateDesperateSaveAttempt.h"

AISystem::AISystem(PathEngine* pathEngine)
	: m_pathEngine(pathEngine)
{
}

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	static float updateTimer = 1.f / 30.f;
	updateTimer -= dt;
	float timePerCell = (GameConstants::CELL_WIDTH / GameConstants::S_SCALE) / GameConstants::PLAYER_SPEED;

	Entity player;
	ComponentHandle<CellComponent> playerCell;
	for (auto& entity : entityManager.entities_with_components<InventoryComponent, CellComponent>())
	{
		if (entity.has_component<AIComponent>())
			continue;

		player = entity;
		playerCell = player.component<CellComponent>();
		break;
	}

	for (auto& entity : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto cell = entity.component<CellComponent>();

		// Reset inputs
		auto input = entity.component<InputComponent>();
		input->moveX = 0.f;
		input->moveY = 0.f;
		input->bombButtonPressed = false;
		input->skillButtonPressed = false;

		if (updateTimer <= 0.f)
		{
			updateTimer = 1.f / 30.f;

			std::vector<Entity> enemies;
			getEnemies(entity, enemies);

			bool found = false;
			bool escape = false;

			RateEscape rateEscape(entity, enemies);
			RateItem rateItem;
			RateCombination<RateEscape, RateItem> rateCombination(rateEscape, rateItem);

			/*
			if (closeEnemies.size() > 0)
			{
				m_pathEngine->searchBest(cell->x, cell->y, m_path, rateCombination, 100);
				escape = true;
			}*/

			Path itemPath;
			m_pathEngine->searchBest(cell->x, cell->y, itemPath, RateCombination<RateItem, RateTrapDanger>(RateItem(), RateTrapDanger(entity, enemies)));

			Path destroyBlockPath;
			m_pathEngine->searchBest(cell->x, cell->y, destroyBlockPath, 
				RateCombination<RateDestroyBlockSpot, RateEscape, RateTrapDanger>(RateDestroyBlockSpot(), RateEscape(entity, enemies), RateTrapDanger(entity, enemies, true)));

			if (!escape)
			{
				if (itemPath.rating > destroyBlockPath.rating)
				{
					m_path = itemPath;
				}
				else
				{
					m_path = destroyBlockPath;
					if (m_path.nodeCount == 1)
					{
						static PlaceBomb placeBomb;
						placeBomb.update(entity);
					}
				}
			}

			if (m_path.nodeCount > 0)
				found = true;

			if (!found && m_pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion)
			{
				m_pathEngine->searchBest(cell->x, cell->y, m_path, RateSafety());

				IsSafePath isSafePath;
				if (!isSafePath(m_path))
				{
					m_pathEngine->searchBest(cell->x, cell->y, m_path, RateDesperateSaveAttempt(), 20);
				}
			}
				
		}

		if (m_path.nodeCount > 1)
		{
			static FollowPath followPath(m_path);
			followPath.setPath(m_path);

			bool cellAffected = m_pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion;
			bool nextCellAffected = m_path.nodes[1]->properties.affectedByExplosion;
			bool nextCellOnFire = m_path.nodes[1]->properties.affectedByExplosion && (m_path.nodes[1]->properties.timeTillExplosion < timePerCell * 1.1f);
			bool cellOnFire = m_path.nodes[0]->properties.affectedByExplosion && (m_path.nodes[0]->properties.timeTillExplosion < timePerCell);

			if (cellOnFire || !nextCellOnFire && (cellAffected || !nextCellAffected))
				followPath.update(entity);
		}
	}
}

void AISystem::visualize()
{
	m_pathEngine->visualize(m_path);
}

void AISystem::getEnemies(Entity self, std::vector<Entity>& outEnemies)
{
	outEnemies.clear();
	for(auto& e : GameGlobals::entities->entities_with_components<InventoryComponent>())
	{
		if (e != self)
			outEnemies.push_back(e);
	}
}

void AISystem::getCloseEnemies(Entity self, std::vector<Entity>& outEnemies)
{
	assert(self.valid() && self.has_component<CellComponent>() && self.has_component<InventoryComponent>());

	auto selfCell = self.component<CellComponent>();
	outEnemies.clear();

	for (auto& e : GameGlobals::entities->entities_with_components<InventoryComponent, CellComponent>())
	{
		auto cell = e.component<CellComponent>();
		uint32_t distance = abs(selfCell->x - cell->x) + abs(selfCell->y - cell->y);
		if (e != self && distance < 4)
			outEnemies.push_back(e);
			
	}
}