#include "BlinkAction.h"

BlinkAction::BlinkAction(PathEngine* pathEngine, PathRating pathRating, Behavior behavior, LayerManager* layerManager)
	: Action(pathEngine, pathRating, behavior, layerManager), m_blinkTime(1.5f), m_remaining(0.f), blinked(false)
{
	setActivationCondition([&](entityx::Entity& entity, float deltaTime) { return isBlinkReady(entity, deltaTime); });
	setRandomPaths(false);
}

void BlinkAction::update(entityx::Entity& entity, float deltaTime)
{
	blinked = true;
	Action::update(entity, deltaTime);
}

bool BlinkAction::isBlinkReady(entityx::Entity& entity, float deltaTime)
{
	auto inventory = entity.component<InventoryComponent>();
	if (!inventory->isActive(SkillType::BLINK))
		return false;

	if (!blinked)
		return true;

	m_remaining -= deltaTime;

	if (m_remaining <= 0.f)
	{
		m_remaining = m_blinkTime;
		blinked = false;
		return true;
	}

	return false;
}