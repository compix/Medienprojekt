#include "AIPath.h"
#include <ostream>
#include <entityx/Entity.h>
#include "../AIUtil.h"

void AIPath::attach(AIPath& path)
{
	for (uint32_t i = 0; i < nodes.size(); ++i)
		nodes.push_back(path.nodes[i]);
	cost += path.cost;
}

bool AIPath::operator==(const AIPath& other)
{
	if (nodes.size() != other.nodes.size())
		return false;

	for (uint32_t i = 0; i < nodes.size(); ++i)
		if (nodes[i] != other.nodes[i])
			return false;

	return true;
}

std::ostream& operator<<(std::ostream& stream, const AIPath& path)
{
	for (uint32_t i = path.curNode; i < path.nodes.size(); ++i)
	{
		auto node = path.nodes[i];
		stream << "(" << int(node->x) << ", " << int(node->y) << ")" << "[" << node->properties.timeTillExplosion << ", " << node->valid << "]"
			<< ((i + 1 < path.nodes.size()) ? " - " : "");
	}

	return stream;
}