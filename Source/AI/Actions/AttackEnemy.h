#pragma once
#include "Action.h"
#include "ActionSelector.h"

/**
* To avoid that AI blinks to often this action adds a time restriction.
*/
class AttackEnemy : public ActionSelector
{
public:
	AttackEnemy(PathEngine* pathEngine, LayerManager* layerManager);
	void preparePath(entityx::Entity& entity) override;

	void update(entityx::Entity& entity, float deltaTime) override;
	bool done() override;
private:
    bool m_throwStarted;
    bool m_placedBomb;
	PathEngine* m_pathEngine;
	ActionPtr m_placeBomb;
	ActionPtr m_throwBomb;
};
