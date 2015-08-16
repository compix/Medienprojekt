#pragma once
#include <cinttypes>
#include <functional>

class PathEngine;
struct GraphNode;
struct AIPath;

// Returns false if the path created with the goal is not valid
// Otherwise pathOut is set and rated
typedef std::function<bool(PathEngine* pathEngine, GraphNode* goal, AIPath& pathOut, uint8_t taskNum)> PathRating;