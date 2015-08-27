#include "LavaSystem.h"
#include "AISystem.h"
#include "../EntityFactory.h"
#include "../Utils/Math.h"
#include "../Components/LavaComponent.h"
#include "../Components/MarkedLavaSpotComponent.h"

LavaSystem::LavaSystem(uint8_t levelWidth, uint8_t levelHeight)
	:m_levelWidth(levelWidth), m_levelHeight(levelHeight), m_totalTime(0.f), m_startTime(120.f), m_lavaSpawnTime(2.f), m_leftTillSpawn(m_lavaSpawnTime),
	m_curCellX(1), m_curCellY(1), m_curDirection(Direction::DOWN), m_topBorder(0), m_botBorder(m_levelHeight-1), m_leftBorder(1), m_rightBorder(m_levelWidth-1),
	m_end(false)
{

}

void LavaSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	if (m_end)
		return;

	m_totalTime += float(dt);

	if (m_totalTime < m_startTime)
		return;

	if (Math::nearEq(m_leftTillSpawn, m_lavaSpawnTime))
		GameGlobals::entityFactory->markLavaSpot(m_curCellX, m_curCellY, m_lavaSpawnTime);

	m_leftTillSpawn -= float(dt);

	if (m_leftTillSpawn <= 0.f)
	{
		if (m_curCellX >= m_levelWidth || m_curCellY >= m_levelHeight)
			return;

		m_leftTillSpawn = m_lavaSpawnTime;
		if (m_curCellX % 2 != 0 || m_curCellY % 2 != 0)
			GameGlobals::entityFactory->createLava(m_curCellX, m_curCellY);

		switch (m_curDirection)
		{
		case Direction::UP:
			--m_curCellY;
			if (m_curCellY == m_topBorder)
			{
				m_curCellY = ++m_topBorder;
				m_curDirection = Direction::LEFT;
				--m_curCellX;
				m_end = m_curCellX == m_leftBorder;
			}
			break;
		case Direction::DOWN:
			++m_curCellY;
			if (m_curCellY == m_botBorder)
			{
				m_curCellY = --m_botBorder;
				m_curDirection = Direction::RIGHT;
				++m_curCellX;
				m_end = m_curCellX == m_rightBorder;
			}
			break;
		case Direction::LEFT:
			--m_curCellX;
			if (m_curCellX == m_leftBorder)
			{
				m_curCellX = ++m_leftBorder;
				m_curDirection = Direction::DOWN;
				++m_curCellY;
				m_end = m_curCellY == m_botBorder;
			}
			break;
		case Direction::RIGHT:
			++m_curCellX;
			if (m_curCellX == m_rightBorder)
			{
				m_curCellX = --m_rightBorder;
				m_curDirection = Direction::UP;
				--m_curCellY;
				m_end = m_curCellY == m_topBorder;
			}
			break;
		default:
			break;
		}
	}
}