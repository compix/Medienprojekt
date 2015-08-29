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
	bool valid(entityx::Entity& entity) override;
	void preparePath(entityx::Entity& entity) override;

	void update(entityx::Entity& entity, float deltaTime) override;
	bool done() override;
private:
	ActionPtr m_placeBomb;
	ActionPtr m_throwBomb;

	bool m_throwStarted;
	bool m_placedBomb;
};