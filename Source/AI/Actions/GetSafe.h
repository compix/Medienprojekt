#pragma once
#include "Action.h"
#include "ActionSelector.h"

/**
* A special selector. This logic needs extra care.
*/
class GetSafe : public ActionSelector
{
public:
	GetSafe(PathEngine* pathEngine, LayerManager* layerManager);

	void preparePath(entityx::Entity& entity) override;
private:
	PathEngine* m_pathEngine;

	ActionPtr m_getSafeAction;
	ActionPtr m_kickBombAction;
	ActionPtr m_tryToSurviveAction;
};