#pragma once
#include <../entityx/entityx/entityx.h>

// Returns true when done
typedef std::function<bool(entityx::Entity& entity)> Behavior;