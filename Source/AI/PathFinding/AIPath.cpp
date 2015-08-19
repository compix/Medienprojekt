#include "AIPath.h"

void AIPath::attach(AIPath& path)
{
	for (int i = 0; i < nodes.size(); ++i)
		nodes.push_back(path.nodes[i]);
	cost += path.cost;
}