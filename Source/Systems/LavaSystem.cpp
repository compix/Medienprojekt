#include "LavaSystem.h"
#include "AISystem.h"
#include "../EntityFactory.h"
#include "../Utils/Math.h"
#include "../Events/Phase2StartedEvent.h"

LavaSystem::LavaSystem(uint8_t levelWidth, uint8_t levelHeight)
	: m_phase2Started(false), m_levelWidth(levelWidth), m_levelHeight(levelHeight), m_timer(0.f), m_startTime(60.f), m_leftTillSpawn(GameConstants::LAVA_SPAWN_TIME),
	  m_topBorder(0), m_botBorder(m_levelHeight-1), m_leftBorder(0), m_rightBorder(m_levelWidth-1)
{

}

void LavaSystem::reset()
{
	m_topBorder   = 0;
	m_botBorder   = m_levelHeight - 1;
	m_leftBorder  = 0;
	m_rightBorder = m_levelWidth - 1;
	m_phase2Started = false;
	m_timer = 0.f;
	m_leftTillSpawn = GameConstants::LAVA_SPAWN_TIME;
}

void LavaSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	m_timer += float(dt);

	if (m_timer < m_startTime)
		return;

	if (!m_phase2Started)
		events.emit<Phase2StartedEvent>();

	m_phase2Started = true;

	bool marked = false;
	if (Math::nearEq(m_leftTillSpawn, GameConstants::LAVA_SPAWN_TIME))
		marked = true;

	m_leftTillSpawn -= float(dt);

	if (!marked && m_leftTillSpawn > 0.f)
		return;

	if (m_leftTillSpawn < 0.f)
		m_leftTillSpawn = GameConstants::LAVA_SPAWN_TIME;

	// Go through top border
	if (m_topBorder + 1 != m_botBorder)
	{
		uint8_t y = marked ? m_topBorder + 1 : ++m_topBorder;
		for (uint8_t x = m_leftBorder + 1; x < m_rightBorder; ++x)
			if (x % 2 != 0 || y % 2 != 0)
			{
				if (marked)
					GameGlobals::entityFactory->markLavaSpot(x, y);
				else
					GameGlobals::entityFactory->createLava(x, y);
			}
				
	}

	// Go through bot border
	if (m_topBorder != m_botBorder - 1)
	{
		uint8_t y = marked ? m_botBorder - 1 : --m_botBorder;
		for (uint8_t x = m_leftBorder + 1; x < m_rightBorder; ++x)
			if (x % 2 != 0 || y % 2 != 0)
			{
				if (marked)
					GameGlobals::entityFactory->markLavaSpot(x, y);
				else
					GameGlobals::entityFactory->createLava(x, y);
			}
	}

	// Go through left border
	if (m_leftBorder + 1 != m_rightBorder)
	{
		uint8_t x = marked ? m_leftBorder + 1 : ++m_leftBorder;
		for (uint8_t y = m_topBorder + 1; y < m_botBorder; ++y)
			if (x % 2 != 0 || y % 2 != 0)
			{
				if (marked)
					GameGlobals::entityFactory->markLavaSpot(x, y);
				else
					GameGlobals::entityFactory->createLava(x, y);
			}
	}

	// Go through right border
	if (m_leftBorder != m_rightBorder - 1)
	{
		uint8_t x = marked ? m_rightBorder - 1 : --m_rightBorder;
		for (uint8_t y = m_topBorder + 1; y < m_botBorder; ++y)
			if (x % 2 != 0 || y % 2 != 0)
			{
				if (marked)
					GameGlobals::entityFactory->markLavaSpot(x, y);
				else
					GameGlobals::entityFactory->createLava(x, y);
			}
	}
}