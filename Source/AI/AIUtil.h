#pragma once
#include <entityx/Entity.h>

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

	/**
	* If portals disappear the path can become invalid. This function checks the path is still valid.
	*/
	bool isValidPath(const AIPath& path);
}
