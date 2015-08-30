#include "AISystem.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../AI/PathFinding/PathEngine.h"
#include "../GameConstants.h"
#include "../AI/PathRatings/RateSafety.h"
#include "../AI/PathRatings/RateItem.h"
#include "../AI/PathRatings/RateDestroyBlockSpot.h"
#include "../AI/PathRatings/RateEscape.h"
#include "../AI/PathRatings/RateCombination.h"
#include "../AI/PathRatings/RateTrapDanger.h"
#include "../AI/PathRatings/RateDistanceToItems.h"
#include "../AI/PathRatings/RateDistanceToAffectedBlocks.h"
#include "../AI/PathRatings/RatePortalSpot.h"
#include "../AI/PathRatings/RateAttackEnemy.h"
#include "../Utils/Logging/Logger.h"
#include <sstream>
#include "../AI/Behaviors/PlaceBomb.h"
#include "../AI/Behaviors/UseSkill.h"
#include "../AI/Behaviors/DoNothing.h"
#include "../Components/ColorComponent.h"
#include "../AI/Actions/GetSafe.h"
#include "../Events/DeathEvent.h"
#include "../Events/BombCreatedEvent.h"
#include "../AI/PathRatings/RateBlink.h"
#include "../AI/Behaviors/UseDirectionSkill.h"
#include "../Components/LayerComponent.h"
#include "../Components/BlockComponent.h"
#include "../AI/Actions/BlinkAction.h"
#include "../AI/Actions/AttackEnemy.h"

AISystem::AISystem(LayerManager* layerManager)
	: m_layerManager(layerManager), m_updateTimer(GameConstants::AI_UPDATE_TIME), m_forceRecomputation(false)
{
	m_pathEngine = std::make_unique<PathEngine>(layerManager);
	m_visualizer.setPathEngine(m_pathEngine.get());
}

void AISystem::init()
{
	bool first = true;
	for (auto entity : GameGlobals::entities->entities_with_components<AIComponent, CellComponent, InputComponent, InventoryComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();
		
		PathRating destroyBlockRating = RateCombination({ RateDestroyBlockSpot(), RateTrapDanger(), RateDistanceToItems() });
		aiComponent->actions[ActionType::DESTROY_BLOCK] = std::make_shared<Action>(m_pathEngine.get(), destroyBlockRating, PlaceBomb(), m_layerManager);

		if (first) // First AI destroys blocks more efficiently
			aiComponent->actions[ActionType::DESTROY_BLOCK]->setRandomPaths(false);
		first = false;

		PathRating placePortalRating = RateCombination({ RateSafety(), RatePortalSpot(), RateTrapDanger() });
		aiComponent->actions[ActionType::PLACE_PORTAL] = std::make_shared<Action>(m_pathEngine.get(), placePortalRating, UseSkill(), m_layerManager);

		aiComponent->actions[ActionType::ATTACK_ENEMY] = std::make_shared<AttackEnemy>(m_pathEngine.get(), m_layerManager);

		PathRating waitRating = RateCombination({ RateSafety(), RateDistanceToAffectedBlocks(), RateTrapDanger() });
		aiComponent->actions[ActionType::WAIT] = std::make_shared<Action>(m_pathEngine.get(), waitRating, DoNothing(), m_layerManager);
		aiComponent->actions[ActionType::WAIT]->setRandomPaths(false);

		PathRating getItemRating = RateCombination({ RateSafety(), RateItem(m_layerManager), RateTrapDanger() });
		aiComponent->actions[ActionType::GET_ITEM] = std::make_shared<Action>(m_pathEngine.get(), getItemRating, DoNothing(), m_layerManager);
		aiComponent->actions[ActionType::GET_ITEM]->setRandomPaths(false);

		aiComponent->actions[ActionType::BLINK] = std::make_shared<BlinkAction>(m_pathEngine.get(), RateBlink(), UseDirectionSkill(), m_layerManager);

		aiComponent->actions[ActionType::GET_SAFE] = std::make_shared<GetSafe>(m_pathEngine.get(), m_layerManager);

		// Set the activation conditions
		aiComponent->actions[ActionType::PLACE_PORTAL]->setActivationCondition([&](entityx::Entity& e, float dt)
		{
			if (e.has_component<InventoryComponent>())
			{
				auto inventory = e.component<InventoryComponent>();
				return inventory->isActive(SkillType::PLACE_PORTAL);
			}

			return false;
		});

		aiComponent->actions[ActionType::WAIT]->setActivationCondition([&](entityx::Entity& e, float dt){return AIUtil::doEntitiesExistWithComponents<BlockComponent>(); });
		aiComponent->actions[ActionType::DESTROY_BLOCK]->setActivationCondition([&](entityx::Entity& e, float dt)
		{
			auto inventory = e.component<InventoryComponent>();
			return !inventory->isHoldingBomb && AIUtil::doEntitiesExistWithComponents<BlockComponent>();
		});
		aiComponent->actions[ActionType::GET_ITEM]->setActivationCondition([&](entityx::Entity& e, float dt){return AIUtil::doEntitiesExistWithComponents<ItemComponent>(); });
	}
}

void AISystem::reset()
{
	m_pathEngine->reset();
}

void AISystem::log(entityx::Entity& entity)
{
	if (!entity)
		return;

	auto aiComponent = entity.component<AIComponent>();
	assert(aiComponent && aiComponent->currentAction);

	if (aiComponent->currentActionType == aiComponent->lastActionType && aiComponent->lastPath == aiComponent->currentAction->path())
		return;

	std::stringstream stream;

	if (aiComponent->currentAction->path().nodes.size() > 0)
	{
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
			stream << aiComponent->currentAction->logString(entity);
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
		case ActionType::ATTACK_ENEMY:
			stream << "Attacking enemy.";
			break;
		case ActionType::BLINK:
			stream << "Blinking.";
			break;
		default:
			stream << "UNKNOWN ACTION.";
			break;
		}
	}
	else
		stream << "No action.";

	auto cell = entity.component<CellComponent>();
	assert(cell);
	auto curOnPath = aiComponent->currentAction->path().current();

	stream << " Pos: (" << int(cell->x) << ", " << int(cell->y) << ")";
	if (curOnPath)
		stream << " PathPos: (" << int(curOnPath->x) << ", " << int(curOnPath->y) << ")";
	stream << " Speed: " << AIUtil::getTimePerCell(entity);
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

void AISystem::updateAIDesires(entityx::Entity& entity, float deltaTime)
{
	auto aiComponent = entity.component<AIComponent>();
	auto& desires = aiComponent->personality.desires;
	auto& changePerSecond = aiComponent->personality.changePerSecond;

	desires.getSafe += changePerSecond.getSafe * deltaTime;
	desires.attackEnemy += changePerSecond.attackEnemy * deltaTime;
	desires.destroyBlock += changePerSecond.destroyBlock * deltaTime;
	desires.getItem += changePerSecond.getItem * deltaTime;
	desires.wait += changePerSecond.wait * deltaTime;
	desires.placePortal += changePerSecond.placePortal * deltaTime;
}

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	m_updateTimer -= static_cast<float>(dt);

	m_pathEngine->update(static_cast<float>(dt));

	for (auto entity : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent, InventoryComponent, TransformComponent, LayerComponent>())
	{
		auto aiComponent = entity.component<AIComponent>();
		auto input = entity.component<InputComponent>();

		updateAIDesires(entity, float(dt));

		m_pathEngine->getGraph()->spreadEnemySmells(entity);
		m_pathEngine->getSimGraph()->spreadEnemySmells(entity);

		// Action activation routine: No need to always run the PathFinding algorithm -> deactivate actions
		// Example: No blocks in level -> No need to find a path to destroy blocks -> deactivate path finding action
		for (auto& pair : aiComponent->actions)
			pair.second->checkForActivation(entity, float(dt));

		// Reset inputs
		input->moveX = 0.f;
		input->moveY = 0.f;
		input->bombButtonPressed = false;
		input->skillButtonPressed = false;

		bool currentActionValid = aiComponent->currentAction ? aiComponent->currentAction->valid(entity) : false;
		if (m_forceRecomputation || !aiComponent->currentAction || (m_updateTimer <= 0.f && (aiComponent->currentAction->done() || !currentActionValid)))
		{
			auto lastAction = aiComponent->currentAction;

			ActionPtr bestAction;
			ActionType bestActionType;

			// Prepare path and choose the best action
			for (auto& pair : aiComponent->actions)
			{
				auto& action = pair.second;

				if (!action->isActive())
					continue;

				action->preparePath(entity);

				if (!bestAction || (bestAction->getRating() < action->getRating()))
				{
					bestAction = action;
					bestActionType = pair.first;
				}
			}

			if (!bestAction)
				continue;

			aiComponent->currentAction = bestAction.get();
			aiComponent->behaviorNode = bestAction->path().behaviorNode;
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

	m_forceRecomputation = false;
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

void AISystem::receive(const PortalCreatedEvent& portalCreatedEvent)
{
	// Portals can cause the paths to be invalid. That's an easy fix for it
	m_forceRecomputation = true;
}