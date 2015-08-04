#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>

class AnimationSystem : public EntitySystem<AnimationSystem>
{
public:
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;
	
	private:
	const std::vector<Entity *> *m_inputEntities;
	const std::vector<Entity *> *m_animationEntities;
};

