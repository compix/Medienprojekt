#include "AISystem.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../AI/PathFinding/PathEngine.h"
#include "../GameConstants.h"
#include "../Utils/Random.h"
#include "../Events/ItemPickedUpEvent.h"
#include "../Game.h"
#include "../Components/DestructionComponent.h"
#include "../Components/BlockComponent.h"
#include "../AI/PathRatings/RateSafety.h"
#include "../AI/PathRatings/RateItem.h"
#include "../AI/PathRatings/RateDestroyBlockSpot.h"
#include "../AI/PathRatings/RateEscape.h"
#include "../AI/PathRatings/RateCombination.h"
#include "../AI/PathRatings/RateTrapDanger.h"
#include "../AI/PathRatings/RateDesperateSaveAttempt.h"
#include "../AI/PathRatings/RateDistanceToItems.h"
#include "../AI/PathRatings/RateDistanceToAffectedBlocks.h"
#include "../AI/PathRatings/RateKickBomb.h"
#include "../AI/PathRatings/RatePortalSpot.h"
#include "../Utils/Logging/Logger.h"
#include <sstream>
#include "../AI/AIUtil.h"
#include "../AI/Behaviors/PlaceBomb.h"
#include "../AI/Behaviors/UseSkill.h"
#include "../AI/Behaviors/FollowPath.h"
#include "../AI/Behaviors/DoNothing.h"
#include "../Components/ColorComponent.h"
#include "../AI/Actions/ActionSelector.h"
#include "../AI/Actions/GetSafe.h"
#include "../Components/PortalComponent.h"

AISystem::AISystem(LayerManager* layerManager)
	: m_layerManager(layerManager), m_updateTimer(GameConstants::AI_UPDATE_TIME), m_invalidPaths(false)
{
	m_pathEngine = std::make_unique<PathEngine>(layerManager);
}

void AISystem::init()
{
	m_pathEngine->getGraph()->init();
	m_pathEngine->getSimGraph()->init();
	
	for (auto entity : GameGlobals::entities->entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();

		PathRating destroyBlockRating = RateCombination({ RateDestroyBlockSpot(), RateEscape(), RateTrapDanger(true), RateDistanceToItems() });
		aiComponent->actions[ActionType::DESTROY_BLOCK] = std::make_shared<Action>(m_pathEngine.get(), destroyBlockRating, PlaceBomb(), m_layerManager);

		PathRating waitRating = RateCombination({ RateSafety(), RateDistanceToAffectedBlocks() });
		aiComponent->actions[ActionType::WAIT] = std::make_shared<Action>(m_pathEngine.get(), waitRating, DoNothing(), m_layerManager);

		PathRating getItemRating = RateCombination({ RateSafety(), RateItem(), RateTrapDanger() });
		aiComponent->actions[ActionType::GET_ITEM] = std::make_shared<Action>(m_pathEngine.get(), getItemRating, DoNothing(), m_layerManager);

		aiComponent->actions[ActionType::GET_SAFE] = std::make_shared<GetSafe>(m_pathEngine.get(), m_layerManager);

		PathRating placePortalRating = RateCombination({ RateSafety(), RatePortalSpot() });
		aiComponent->actions[ActionType::PLACE_PORTAL] = std::make_shared<Action>(m_pathEngine.get(), placePortalRating, UseSkill(), m_layerManager);
	}
}

void AISystem::logAction(LogServiceId serviceId, ActionType action, AIPath& path)
{
	std::stringstream ss;

	switch (action)
	{
	case ActionType::DESTROY_BLOCK:
		ss << "Destroying block.";
		break;
	case ActionType::WAIT:
		ss << "Waiting for opportunities.";
		break;
	case ActionType::GET_ITEM:
		ss << "Getting item.";
		break;
	case ActionType::GET_SAFE:
		ss << "Getting safe.";
		break;
	case ActionType::KICK_BOMB:
		ss << "Kicking bomb.";
		break;
	case ActionType::TRY_TO_SURVIVE:
		ss << "Trying to survive.";
		break;
	case ActionType::PLACE_PORTAL:
		ss << "Placing portal.";
		break;
	default:
		ss << "UNKNOWN ACTION.";
		break;
	}

	if (path.goal())
	{
		ss << " Goal: x = " << static_cast<int>(path.goal()->x) << " y = " << static_cast<int>(path.goal()->y);
	}

	Logger::log(ss.str(), serviceId);
}

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{

	m_updateTimer -= dt;

	m_pathEngine->update(static_cast<float>(dt));

	for (auto entity : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto cell = entity.component<CellComponent>();
		auto aiComponent = entity.component<AIComponent>();
		auto input = entity.component<InputComponent>();
		auto color = entity.component<ColorComponent>();

		// Reset inputs
		input->moveX = 0.f;
		input->moveY = 0.f;
		input->bombButtonPressed = false;
		input->skillButtonPressed = false;

		if (m_invalidPaths || !aiComponent->currentAction || aiComponent->currentAction->done() || !aiComponent->currentAction->valid(entity))
		{
			ActionPtr bestAction;
			ActionType bestActionType;

			// Prepare path and choose the best action
			for (auto& pair : aiComponent->actions)
			{
				auto& action = pair.second;
				action->preparePath(entity);

				if (!bestAction || bestAction->getRating() < action->getRating())
				{
					bestAction = action;
					bestActionType = pair.first;
				}
			}

			assert(bestAction);
			aiComponent->currentAction = bestAction.get();

			// Logging
			std::stringstream ss;
			ss << "AI/Entity" << color->color.str() << ".log";
			LogServiceId logId = Logger::requestService(ss.str());

			logAction(logId, bestActionType, aiComponent->currentAction->path());

			m_invalidPaths = false;
		}

		if (aiComponent->currentAction->path().nodes.size() == 0)
		{
			// No viable action
			continue;
		}

		if (!AIUtil::isOnPath(entity, aiComponent->currentAction->path()))
		{
			std::cout << "WHY YOU NOT ON PATH?" << std::endl;
		}

		aiComponent->currentAction->update(entity, static_cast<float>(dt));
		m_pathEngine->visualize(aiComponent->currentAction->path());
	}

	if (m_updateTimer <= 0.f)
		m_updateTimer = GameConstants::AI_UPDATE_TIME;

	visualize();
}

void AISystem::configure(entityx::EventManager& eventManager)
{
	eventManager.subscribe<entityx::EntityDestroyedEvent>(*this);
}

void AISystem::receive(const entityx::EntityDestroyedEvent& destroyedEvent)
{
	auto entity = destroyedEvent.entity;
	if (entity.has_component<PortalComponent>())
	{
		// If portals disappear then paths can become invalid -> easy fix: force a recomputation
		m_invalidPaths = true;
	}
}

void AISystem::visualize()
{
	m_pathEngine->visualize();
}

void AISystem::getEnemies(Entity self, std::vector<Entity>& outEnemies)
{
	outEnemies.clear();
	for(auto e : GameGlobals::entities->entities_with_components<InventoryComponent>())
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

	for (auto e : GameGlobals::entities->entities_with_components<InventoryComponent, CellComponent>())
	{
		auto cell = e.component<CellComponent>();
		uint32_t distance = abs(selfCell->x - cell->x) + abs(selfCell->y - cell->y);
		if (e != self && distance < 4)
			outEnemies.push_back(e);
			
	}
}