#include "EntityLayer.h"
#include <iostream>

EntityLayer::EntityLayer(int width, int height, int value)
	: m_width(width), m_height(height), m_value(value)
{
	m_grid = new EntityCollection*[width];
	for (int i = 0; i < width; i++)
		m_grid[i] = new EntityCollection[height];

}

EntityLayer::~EntityLayer()
{
	for (int i = 0; i < m_width; i++)
		delete[] m_grid[i];

	delete[] m_grid;
}

void EntityLayer::add(Entity *entity, int cellX, int cellY)
{
	if (cellX < 0 || cellX >= m_width || cellY < 0 || cellY >= m_height)
	{
		std::cout << "Could not add entity. Reason: cell(" << cellX << ", " << cellY << ") is out of bounds." << std::endl;
		return;
	}

	m_grid[cellX][cellY].push_back(entity);

	for (auto& l : m_changeListeners)
		l->onEntityAdded(entity);
}

void EntityLayer::remove(Entity *entity, int cellX, int cellY)
{
	if (cellX < 0 || cellX >= m_width || cellY < 0 || cellY >= m_height)
	{
		std::cout << "Could not remove entity. Reason: cell(" << cellX << ", " << cellY << ") is out of bounds." << std::endl;
		return;
	}

	EntityCollection& entityCollection = m_grid[cellX][cellY];
	entityCollection.erase(std::remove(entityCollection.begin(), entityCollection.end(), entity), entityCollection.end());

	for (auto& l : m_changeListeners)
		l->onEntityRemoved(entity);
}

void EntityLayer::stopListening(IOnChangeListener* listener)
{
	auto it = m_changeListeners.begin();
	for (; it != m_changeListeners.end(); ++it)
	{
		if (*it == listener)
		{
			m_changeListeners.erase(it);
			return;
		}
	}

	std::cout << "EntityLayer - WARNING: Attempt to remove a change listener failed. Reason: There is no such listener.\n";
}