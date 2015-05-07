#include "EntityLayer.h"

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

void EntityLayer::add(Entity entity, int cellX, int cellY)
{
	if (cellX < 0 || cellX >= m_width || cellY < 0 || cellY >= m_height)
	{
		std::cout << "Could not add entity. Reason: cell(" << cellX << ", " << cellY << ") is out of bounds." << std::endl;
		return;
	}

	m_grid[cellX][cellY].push_back(entity);
}

void EntityLayer::remove(Entity entity, int cellX, int cellY)
{
	if (cellX < 0 || cellX >= m_width || cellY < 0 || cellY >= m_height)
	{
		std::cout << "Could not remove entity. Reason: cell(" << cellX << ", " << cellY << ") is out of bounds." << std::endl;
		return;
	}

	EntityCollection& entityCollection = m_grid[cellX][cellY];
	entityCollection.erase(std::remove(entityCollection.begin(), entityCollection.end(), entity), entityCollection.end());
}