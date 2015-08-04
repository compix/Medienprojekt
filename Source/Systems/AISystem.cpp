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
#include "../AI/Checks/AIUtil.h"
#include "../AI/PathRatings/RateDistanceToItems.h"
#include "../AI/PathRatings/RateDistanceToAffectedBlocks.h"
#include "../AI/PathRatings/RateKickBomb.h"
#include "../AI/PathRatings/RatePortalSpot.h"
#include "../AI/UseSkill.h"
#include "../Utils/Logging/Logger.h"
#include <sstream>

AISystem::AISystem(LayerManager* layerManager)
	: m_layerManager(layerManager)
{
	m_pathEngine = std::make_unique<PathEngine>(layerManager);
}

void AISystem::init()
{
	m_pathEngine->getGraph()->init();
	m_pathEngine->getSimGraph()->init();
}

void AISystem::logAction(LogServiceId serviceId, AIAction action)
{
	switch (action)
	{
	case AIAction::PORTAL_PATH:
		Logger::log("Taking path to place portal.", serviceId);
		break;
	case AIAction::WAIT_PATH:
		Logger::log("Taking path to wait for block destruction.", serviceId);
		break;
	case AIAction::ITEM_PATH:
		Logger::log("Taking path to get item.", serviceId);
		break;
	case AIAction::DESTROY_BLOCK_PATH:
		Logger::log("Taking path to place a bomb to destroy blocks.", serviceId);
		break;
	case AIAction::SAFE_PATH:
		Logger::log("Taking path to be safe.", serviceId);
		break;
	case AIAction::KICK_BOMB_PATH:
		Logger::log("Taking path to kick a bomb to open escape path.", serviceId);
		break;
	case AIAction::DESPERATE_SAFE_PATH:
		Logger::log("Desperately trying to survive.", serviceId);
		break;
	case AIAction::PLACING_BOMB:
		Logger::log("Placing bomb.", serviceId);
		break;
	case AIAction::PLACING_PORTAL:
		Logger::log("Placing portal.", serviceId);
		break;
	default:
		Logger::log("UNKNOWN ACTION.", serviceId);
		break;
	}
}

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	static float updateTimer = 1.f / 30.f;
	updateTimer -= dt;

	m_pathEngine->update(static_cast<float>(dt));

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
		auto aiComponent = entity.component<AIComponent>();

		std::stringstream ss;
		ss << "AI/Entity" << static_cast<int>(aiComponent->id) << ".log";
		LogServiceId logId = Logger::requestService(ss.str());

		float timePerCell = AIUtil::getTimePerCell(entity);

		// Reset inputs
		auto input = entity.component<InputComponent>();
		input->moveX = 0.f;
		input->moveY = 0.f;
		input->bombButtonPressed = false;
		input->skillButtonPressed = false;


		Path& path = aiComponent->path;

		if (updateTimer <= 0.f)
		{
			GraphNode* lastAIGoal = aiComponent->path.nodeCount > 0 ? aiComponent->path.nodes[aiComponent->path.nodeCount - 1] : nullptr;
			AIAction newAction;

			std::vector<Entity> enemies;
			getEnemies(entity, enemies);

			bool found = false;
			bool escape = false;

			Path portalPath;
			m_pathEngine->searchBest(cell->x, cell->y, portalPath, RateCombination<RateSafety, RatePortalSpot>(RateSafety(entity), RatePortalSpot(entity)));

			Path waitPath;
			m_pathEngine->searchBest(cell->x, cell->y, waitPath, RateCombination<RateSafety, RateDistanceToAffectedBlocks>(RateSafety(entity), RateDistanceToAffectedBlocks(entity)));

			Path itemPath;
			m_pathEngine->searchBest(cell->x, cell->y, itemPath, RateCombination<RateItem, RateTrapDanger>(RateItem(entity), RateTrapDanger(entity, enemies)));

			Path destroyBlockPath;
			m_pathEngine->searchBest(cell->x, cell->y, destroyBlockPath, 
				RateCombination<RateDestroyBlockSpot, RateEscape, RateTrapDanger, RateDistanceToItems>
				(RateDestroyBlockSpot(entity), RateEscape(entity, enemies), RateTrapDanger(entity, enemies, true), RateDistanceToItems(entity)));

			IsSafePath isSafePath;

			if (!escape)
			{
				if (itemPath.rating > destroyBlockPath.rating)
				{
					path = itemPath;
					newAction = AIAction::ITEM_PATH;
				}
				else if (waitPath.rating > destroyBlockPath.rating && isSafePath(entity, waitPath))
				{
					path = waitPath;
					newAction = AIAction::WAIT_PATH;
				} 
				else if (portalPath.rating > destroyBlockPath.rating && isSafePath(entity, portalPath))
				{
					path = portalPath;
					newAction = AIAction::PORTAL_PATH;
					if (path.nodeCount == 1)
					{
						UseSkill useSkill;
						useSkill.update(entity);
						newAction = AIAction::PLACING_PORTAL;
					}
				} else
				{
					path = destroyBlockPath;
					newAction = AIAction::DESTROY_BLOCK_PATH;
					if (path.nodeCount == 1)
					{
						PlaceBomb placeBomb;
						placeBomb.update(entity);
						newAction = AIAction::PLACING_BOMB;
					}
				}
			}

			if (path.nodeCount > 0)
				found = true;

			if (!found && m_pathEngine->getGraph()->getNode(cell->x, cell->y)->properties.affectedByExplosion)
			{
				m_pathEngine->searchBest(cell->x, cell->y, path, RateSafety(entity));
				newAction = AIAction::SAFE_PATH;

				float minExploTime;
				isSafePath(entity, path, &minExploTime);

				if (path.nodeCount == 0)
				{
					m_pathEngine->searchBest(cell->x, cell->y, path, RateKickBomb(entity));
					newAction = AIAction::KICK_BOMB_PATH;
				}
				else if (minExploTime <= timePerCell*0.5f)
				{
					m_pathEngine->searchBest(cell->x, cell->y, path, RateDesperateSaveAttempt(entity), 20);
					newAction = AIAction::DESPERATE_SAFE_PATH;
				}
			}
			
			GraphNode* newAIGoal = path.nodeCount > 0 ? path.nodes[path.nodeCount - 1] : nullptr;
			if (newAIGoal != lastAIGoal || newAction != aiComponent->action)
			{
				// Goal of AI changed: Log the action
				logAction(logId, newAction);
			}

			aiComponent->action = newAction;
		}

		if (path.nodeCount > 1)
		{
			FollowPath followPath(path, m_layerManager);
			followPath.update(entity);
		}

		m_pathEngine->visualize(aiComponent->path);
	}

	if (updateTimer <= 0.f)
	{
		updateTimer = 1.f / 30.f;
	}

	visualize();
}

void AISystem::visualize()
{
	m_pathEngine->visualize();
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