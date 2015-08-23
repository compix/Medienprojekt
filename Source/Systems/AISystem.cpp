#include "AISystem.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../AI/PathFinding/PathEngine.h"
#include "../GameConstants.h"
#include "../Game.h"
#include "../AI/PathRatings/RateSafety.h"
#include "../AI/PathRatings/RateItem.h"
#include "../AI/PathRatings/RateDestroyBlockSpot.h"
#include "../AI/PathRatings/RateEscape.h"
#include "../AI/PathRatings/RateCombination.h"
#include "../AI/PathRatings/RateTrapDanger.h"
#include "../AI/PathRatings/RateDistanceToItems.h"
#include "../AI/PathRatings/RateDistanceToAffectedBlocks.h"
#include "../AI/PathRatings/RatePortalSpot.h"
#include "../Utils/Logging/Logger.h"
#include <sstream>
#include "../AI/Behaviors/PlaceBomb.h"
#include "../AI/Behaviors/UseSkill.h"
#include "../AI/Behaviors/DoNothing.h"
#include "../Components/ColorComponent.h"
#include "../AI/Actions/GetSafe.h"
#include "../Events/DeathEvent.h"
#include "../Events/BombCreatedEvent.h"

AISystem::AISystem(LayerManager* layerManager)
	: m_layerManager(layerManager), m_updateTimer(GameConstants::AI_UPDATE_TIME)
{
	m_pathEngine = std::make_unique<PathEngine>(layerManager);
	m_visualizer.setPathEngine(m_pathEngine.get());
}

void AISystem::init()
{
	for (auto entity : GameGlobals::entities->entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();

		PathRating destroyBlockRating = RateCombination({ RateDestroyBlockSpot(), RateEscape(), RateTrapDanger(), RateDistanceToItems() });
		aiComponent->actions[ActionType::DESTROY_BLOCK] = std::make_shared<Action>(m_pathEngine.get(), destroyBlockRating, PlaceBomb(), m_layerManager);

		PathRating waitRating = RateCombination({ RateSafety(), RateDistanceToAffectedBlocks(), RateTrapDanger() });
		aiComponent->actions[ActionType::WAIT] = std::make_shared<Action>(m_pathEngine.get(), waitRating, DoNothing(), m_layerManager);

		PathRating getItemRating = RateCombination({ RateSafety(), RateItem(), RateTrapDanger() });
		aiComponent->actions[ActionType::GET_ITEM] = std::make_shared<Action>(m_pathEngine.get(), getItemRating, DoNothing(), m_layerManager);

		aiComponent->actions[ActionType::GET_SAFE] = std::make_shared<GetSafe>(m_pathEngine.get(), m_layerManager);

		PathRating placePortalRating = RateCombination({ RateSafety(), RatePortalSpot(), RateTrapDanger() });
		aiComponent->actions[ActionType::PLACE_PORTAL] = std::make_shared<Action>(m_pathEngine.get(), placePortalRating, UseSkill(), m_layerManager);
	}
}

void AISystem::reset()
{
	m_pathEngine->reset();
}

void AISystem::log(entityx::Entity& entity)
{
	auto aiComponent = entity.component<AIComponent>();
	assert(aiComponent && aiComponent->currentAction);

	if (aiComponent->currentActionType == aiComponent->lastActionType && aiComponent->lastPath == aiComponent->currentAction->path())
		return;

	if (aiComponent->currentAction->path().nodes.size() == 0)
		log(entity, "No action");

	std::stringstream stream;

	switch (aiComponent->currentActionType)
	{
	case ActionType::DESTROY_BLOCK:
		stream << "Destroying block.";	
		break;
	case ActionType::WAIT:
		stream << "Waiting for opportunities.";
		break;
	case ActionType::GET_ITEM:
		stream << "Getting item.";
		break;
	case ActionType::GET_SAFE:
		stream << "Getting safe.";
		break;
	case ActionType::KICK_BOMB:
		stream << "Kicking bomb.";
		break;
	case ActionType::TRY_TO_SURVIVE:
		stream << "Trying to survive.";
		break;
	case ActionType::PLACE_PORTAL:
		stream << "Placing portal.";
		break;
	default:
		stream << "UNKNOWN ACTION.";
		break;
	}

	stream << " Path: " << aiComponent->currentAction->path();

	log(entity, stream.str());
}

void AISystem::log(entityx::Entity& entity, const std::string& txt)
{
	auto color = entity.component<ColorComponent>();
	assert(color);

	std::stringstream serviceStream;
	serviceStream << "AI/Entity" << color->color.str() << ".log";
	LogServiceId serviceId = Logger::requestService(serviceStream.str());

	Logger::log(txt, serviceId);
}

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	m_updateTimer -= static_cast<float>(dt);

	m_pathEngine->update(static_cast<float>(dt));

	for (auto entity : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();
		auto input = entity.component<InputComponent>();

		// Reset inputs
		input->moveX = 0.f;
		input->moveY = 0.f;
		input->bombButtonPressed = false;
		input->skillButtonPressed = false;

		if (!aiComponent->currentAction || (m_updateTimer <= 0.f && (aiComponent->currentAction->done() || !aiComponent->currentAction->valid(entity))))
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
			aiComponent->currentActionType = bestActionType;

#ifdef AI_LOGGING
			log(entity);
#endif

			aiComponent->lastActionType = aiComponent->currentActionType;
			aiComponent->lastPath = aiComponent->currentAction->path();
		}

		if (!aiComponent->currentAction || aiComponent->currentAction->path().nodes.size() == 0)
			continue; // No viable action

		aiComponent->currentAction->update(entity, static_cast<float>(dt));
	}

	m_visualizer.visualize(static_cast<float>(dt));

	if (m_updateTimer <= 0.f)
		m_updateTimer = GameConstants::AI_UPDATE_TIME;
}

void AISystem::configure(entityx::EventManager& eventManager)
{
#ifdef AI_LOGGING
	eventManager.subscribe<DeathEvent>(*this);
	eventManager.subscribe<BombCreatedEvent>(*this);
#endif
}

void AISystem::receive(const DeathEvent& deathEvent)
{
	auto entity = deathEvent.dyingEntity;
	if (entity && entity.has_component<AIComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();
		std::stringstream ss;
		ss << "DEAD";
		if (aiComponent->currentAction && aiComponent->currentAction->path().nodes.size() > 0)
			ss << " Path: " << aiComponent->currentAction->path();
		else
			ss << " No last action :(";

		log(entity, ss.str());
	}
}

void AISystem::receive(const BombCreatedEvent& bombCreatedEvent)
{
	auto bomb = bombCreatedEvent.entity;
	if (bomb && bomb.has_component<CellComponent>())
	{
		auto cell = bomb.component<CellComponent>();
		for (auto ai : GameGlobals::entities->entities_with_components<AIComponent>())
		{
			std::stringstream ss;
			if (ai == bombCreatedEvent.owner)
				ss << "I placed a bomb at (";
			else
				ss << "Someone placed a bomb at (";

			ss << int(cell->x) << ", " << int(cell->y) << ")";
			log(ai, ss.str());
		}
	}
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