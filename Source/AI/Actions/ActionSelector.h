#pragma once
#include "Action.h"

class ActionSelector : public IAction
{
public:
	ActionSelector() : m_currentAction(nullptr) {}
	ActionSelector(std::initializer_list<ActionPtr> actions);

	inline void setActions(std::initializer_list<ActionPtr> actions) { m_actions = actions; }
	bool valid(entityx::Entity& entity) override;
	void update(entityx::Entity& entity, float deltaTime) override;
	bool done() override;
	float getRating() override;
	AIPath& path() override;
	void preparePath(entityx::Entity& entity) override;

	void resetRating() override;

protected:
	std::vector<ActionPtr> m_actions;
	IAction* m_currentAction;
};