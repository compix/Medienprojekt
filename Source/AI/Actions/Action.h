#pragma once
#include "../PathRatings/PathRating.h"
#include "../Behaviors/Behavior.h"
#include "../PathFinding/AIPath.h"
#include "../Behaviors/FollowPath.h"
#include <entityx/entityx.h>

class BaseAction
{
public:
	BaseAction() : m_numOfChecks(5), m_randomPaths(true), m_resting(false) {}
	virtual ~BaseAction() {}

	virtual bool valid(entityx::Entity& entity) = 0;
	virtual void update(entityx::Entity& entity, float deltaTime) = 0;
	virtual bool done() = 0;
	virtual float getRating() = 0;
	virtual void resetRating() = 0;
	virtual AIPath& path() = 0;
	virtual void preparePath(entityx::Entity& entity) = 0;
	virtual inline void setNumOfChecks(uint8_t num) { m_numOfChecks = num; };
	virtual inline void setRandomPaths(bool randomPaths) { m_randomPaths = randomPaths; }
	virtual inline void rest() { m_resting = true; }

	virtual std::string logString(entityx::Entity& entity) { return ""; }
protected:
	uint8_t m_numOfChecks;
	uint8_t m_randomPaths;
	bool m_resting;
};

typedef std::shared_ptr<BaseAction> ActionPtr;

class Action : public BaseAction
{
public:
	Action();
	Action(PathEngine* pathEngine, PathRating pathRating, Behavior behavior, LayerManager* layerManager);

	/**
	* Check if the action is still valid according to its path rating.
	*/
	bool valid(entityx::Entity& entity) override;

	void update(entityx::Entity& entity, float deltaTime) override;

	inline bool done() override { return m_resting || m_behaviorExecuted; }

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