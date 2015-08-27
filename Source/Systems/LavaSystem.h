#pragma once
#include <entityx/System.h>
#include "../Components/DirectionComponent.h"

class LavaSystem : public entityx::System<LavaSystem>
{
public:
	LavaSystem(uint8_t levelWidth, uint8_t levelHeight);
	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

private:
	bool m_phase2Started;

	uint8_t m_levelWidth;
	uint8_t m_levelHeight;

	float m_totalTime;
	const float m_startTime;
	float m_leftTillSpawn;

	uint8_t m_curCellX;
	uint8_t m_curCellY;
	Direction m_curDirection;

	uint8_t m_topBorder;
	uint8_t m_botBorder;
	uint8_t m_leftBorder;
	uint8_t m_rightBorder;

	bool m_end;
};