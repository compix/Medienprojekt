#pragma once
#include <entityx/Entity.h>
#include "../GameGlobals.h"

struct AIPath;

namespace AIUtil
{
	float getTimePerCell(entityx::Entity& entity);
	bool isSafePath(entityx::Entity& entity, AIPath& path, float* minExplosionTime = nullptr);
	bool isOnPath(entityx::Entity& entity, const AIPath& path);

	/**
	* Checks if the path is blocked. Ignores the last node because there could be a bomb that the AI wants to kick.
	*/
	bool isBlockedIgnoreLast(const AIPath& path);
	bool isBlocked(const AIPath& path);

	/**
	* If portals disappear the path can become invalid. This function checks the path is still valid.
	*/
	bool isValidPath(const AIPath& path);

	uint8_t distanceToClosestEnemy(entityx::Entity& self, uint8_t x, uint8_t y);
	uint8_t distanceToClosestEnemy(uint8_t x, uint8_t y, std::vector<entityx::Entity>& enemies);
	void getEnemies(entityx::Entity& self, std::vector<entityx::Entity>& outEnemies);

	template<class C, class... Args>
	static bool doEntitiesExistWithComponents();
}

template<class C, class... Args>
static bool AIUtil::doEntitiesExistWithComponents()
{
	for (auto entity : GameGlobals::entities->entities_with_components<C, Args...>())
		return true;

	return false;
}
