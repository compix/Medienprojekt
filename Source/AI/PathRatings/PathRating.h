#pragma once
#include <functional>
#include <entityx/entityx.h>

class PathEngine;
struct GraphNode;
struct AIPath;

/**
* Returns false if the path is not valid.
* Otherwise the given path will be rated.
*/
typedef std::function<bool(PathEngine* pathEngine, AIPath& path, entityx::Entity& entity, uint8_t taskNum)> PathRating;