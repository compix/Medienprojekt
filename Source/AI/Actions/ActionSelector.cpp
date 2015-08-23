#include "ActionSelector.h"

ActionSelector::ActionSelector(std::initializer_list<ActionPtr> actions)
	:m_actions(actions), m_currentAction(nullptr)
{
}

bool ActionSelector::valid(entityx::Entity& entity)
{
	return m_currentAction && m_currentAction->valid(entity);
}

void ActionSelector::update(entityx::Entity& entity, float deltaTime)
{
	if (m_currentAction)
	{
		m_currentAction->update(entity, deltaTime);
	}
}

bool ActionSelector::done()
{
	return m_currentAction && m_currentAction->done();
}

float ActionSelector::getRating()
{
	return m_currentAction ? m_currentAction->getRating() : -FLT_MAX;
}

AIPath& ActionSelector::path()
{
	return m_currentAction ? m_currentAction->path() : m_emptyPath;
}

void ActionSelector::preparePath(entityx::Entity& entity)
{
	// Select the first valid action
	for (auto& action : m_actions)
	{
		action->preparePath(entity);
		if (!m_currentAction || action->getRating() > 0.f)
		{
			m_currentAction = action.get();
			return;
		}
	}
}

void ActionSelector::resetRating()
{
	assert(m_currentAction);
	m_currentAction->resetRating();
}