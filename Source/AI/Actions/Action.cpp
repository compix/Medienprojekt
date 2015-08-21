#include "Action.h"
#include "../../Components/CellComponent.h"
#include "../../Components/AIComponent.h"
#include "../AIUtil.h"

Action::Action()
{
}

Action::Action(PathEngine* pathEngine, PathRating pathRating, Behavior behavior, LayerManager* layerManager)
	: m_pathEngine(pathEngine), m_pathRating(pathRating), m_behavior(behavior), m_followPath(layerManager), m_behaviorExecuted(false)
{
}

bool Action::valid(entityx::Entity& entity)
{
	return m_followPath.path().nodes.size() > 0 && 
		  !AIUtil::isBlockedIgnoreLast(m_followPath.path()) && 
		  AIUtil::isValidPath(m_followPath.path()) && 
		  m_pathRating(m_pathEngine, m_followPath.path(), entity);
}

void Action::update(entityx::Entity& entity, float deltaTime)
{
	assert(entity && entity.has_component<CellComponent>() && entity.has_component<TransformComponent>() && entity.has_component<AIComponent>());

	m_followPath(entity);

	if (!m_behaviorExecuted && m_followPath.path().reachedGoal())
	{
		m_behavior(entity);
		m_behaviorExecuted = true;
	}
}

void Action::preparePath(entityx::Entity& entity)
{
	assert(entity && entity.has_component<CellComponent>() && entity.has_component<TransformComponent>() && entity.has_component<AIComponent>());

	auto cell = entity.component<CellComponent>();
	AIPath path;
	m_pathEngine->searchBest(entity, cell->x, cell->y, path, m_pathRating);

	m_followPath.setPath(path);

	m_behaviorExecuted = false;
}