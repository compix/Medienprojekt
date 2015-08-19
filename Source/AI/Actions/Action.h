#pragma once
#include "../PathRatings/PathRating.h"
#include "../Behaviors/Behavior.h"
#include "../PathFinding/AIPath.h"
#include "../Behaviors/FollowPath.h"
#include <entityx/entityx.h>

class IAction
{
public:
	virtual ~IAction() {}

	virtual bool valid(entityx::Entity& entity) = 0;
	virtual void update(entityx::Entity& entity, float deltaTime) = 0;
	virtual bool done() = 0;
	virtual float getRating() = 0;
	virtual void resetRating() = 0;
	virtual AIPath& path() = 0;
	virtual void preparePath(entityx::Entity& entity) = 0;
};

typedef std::shared_ptr<IAction> ActionPtr;

class Action : public IAction
{
public:
	Action();
	Action(PathEngine* pathEngine, PathRating pathRating, Behavior behavior, LayerManager* layerManager);

	/**
	* Check if the action is still valid according to its path rating.
	*/
	bool valid(entityx::Entity& entity) override;

	void update(entityx::Entity& entity, float deltaTime) override;

	inline bool done() override { return m_behaviorExecuted; }

	inline float getRating() override { return m_followPath.path().rating; }
	inline void resetRating() override { m_followPath.path().resetRating(); }
	inline AIPath& path() override { return m_followPath.path(); }

	/**
	* Compute the best path with the given path rating and prepare it for update.
	*/
	void preparePath(entityx::Entity& entity) override;
private:
	PathEngine* m_pathEngine;
	PathRating m_pathRating;
	Behavior m_behavior;
	FollowPath m_followPath;

	bool m_behaviorExecuted;
};