#pragma once
#include <entityx/Entity.h>

struct AIPath;

namespace AIUtil
{
	float getTimePerCell(entityx::Entity& entity);
	bool isSafePath(entityx::Entity& entity, AIPath& path, float* minExplosionTime = nullptr);
}
