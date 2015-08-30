#pragma once
#include "../PathRatings/PathRating.h"
#include "../Behaviors/Behavior.h"
#include "../PathFinding/AIPath.h"
#include "../Behaviors/FollowPath.h"
#include <entityx/entityx.h>

/**
* Functor for the action activations. If this condition returns true then the action will be activated and considered in the computation.
*/
typedef std::function<bool(entityx::Entity& entity, float deltaTime)> ActionActivationCondition;

class AbstractAction
{
public:
	AbstractAction() : m_numOfPaths(5), m_randomPaths(true),
		m_activationCondition([](entityx::Entity& e, float dt){ return true; }) {}

	virtual ~AbstractAction() {}

	virtual bool valid(entityx::Entity& entity) = 0;
	virtual void update(entityx::Entity& entity, float deltaTime) = 0;
	virtual bool done() = 0;
	virtual float getRating() = 0;
	virtual void resetRating() = 0;
	virtual AIPath& path() = 0;
	virtual void preparePath(entityx::Entity& entity) = 0;
	virtual inline void setNumOfChecks(uint8_t num) { m_numOfPaths = num; };
	virtual inline void setRandomPaths(bool randomPaths) { m_randomPaths = randomPaths; }
	virtual inline void activate(bool active) { m_active = active; }
	virtual inline bool isActive() { return m_active; }
	virtual inline void setActivationCondition(ActionActivationCondition actionActivationCondition) { m_activationCondition = actionActivationCondition; }
	
	virtual inline void checkForActivation(entityx::Entity& entity, float deltaTime) { m_active = m_activationCondition(entity, deltaTime); }

	virtual std::string logString(entityx::Entity& entity) { return ""; }
protected:
	uint8_t m_numOfPaths; // How many valid paths to consider (no need to find all viable paths)
	bool m_randomPaths; // true: a random path will be chosen, false: the best path will be chosen
	bool m_active; // Indicates if the action is currently active. Inactive actions won't be executed.

	// The condition to activate/deactivate actions
	ActionActivationCondition m_activationCondition;
};

typedef std::shared_ptr<AbstractAction> ActionPtr;

class Action : public AbstractAction
{
public:
	Action();
	Action(PathEngine* pathEngine, PathRating pathRating, Behavior behavior, LayerManager* layerManager);

	/**
	* Check if the action is still valid according to its path rating.
	*/
	bool valid(entityx::Entity& entity) override;

	/**
	* Path following and behavior execution.
	*/
	void update(entityx::Entity& entity, float deltaTime) override;

	inline bool done() override { return m_behaviorExecuted; }

	inline float getRating() override { return m_followPath.path().rating; }
	inline void resetRating() override { m_followPath.path().resetRating(); }
	inline AIPath& path() override { return m_followPath.path(); }
	
	/**
	* Compute the best or a random path with the given path rating and prepare it for update.
	*/
	void preparePath(entityx::Entity& entity) override;

protected:
	PathEngine* m_pathEngine;
	PathRating m_pathRating;
	Behavior m_behavior;
	FollowPath m_followPath;

	bool m_behaviorExecuted;
};