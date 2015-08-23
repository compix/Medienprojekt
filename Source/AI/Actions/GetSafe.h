#pragma once
#include "Action.h"
#include "ActionSelector.h"

/**
* A special selector. This logic needs extra care.
*/
class GetSafe : public ActionSelector
{
public:
	bool done() override;
public:
	GetSafe(PathEngine* pathEngine, LayerManager* layerManager);

	void preparePath(entityx::Entity& entity) override;

	void update(entityx::Entity& entity, float deltaTime) override;
private:
	PathEngine* m_pathEngine;

	ActionPtr m_getSafeAction;
	ActionPtr m_kickBombAction;
	ActionPtr m_tryToSurviveAction;
	ActionPtr m_punchBomb;
	
	const float m_waitTime;
	float m_waitTimer;
};