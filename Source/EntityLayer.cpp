#include "EntityLayer.h"
#include <algorithm>

EntityLayer::EntityLayer(int value, bool _static)
	:m_value(value), m_static(_static)
{

}

void EntityLayer::add(Entity entity)
{
	m_entities.push_back(entity);
}

void EntityLayer::remove(Entity entity)
{
	std::remove(m_entities.begin(), m_entities.end(), entity);
}
