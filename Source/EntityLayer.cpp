#include "EntityLayer.h"

EntityLayer::EntityLayer(uint8_t width, uint8_t height, int value)
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

void EntityLayer::reset()
{
	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			m_grid[x][y].clear();
		}
	}
}

void EntityLayer::add(Entity entity, uint8_t cellX, uint8_t cellY)
{
	if (!inBounds(cellX, cellY))
	{
		std::cout << "Could not add entity. Reason: cell(" << cellX << ", " << cellY << ") is out of bounds." << std::endl;
		return;
	}

	m_grid[cellX][cellY].push_back(entity);

	for (auto& l : m_changeListeners)
		l->onEntityAdded(entity);
}

void EntityLayer::remove(Entity entity, uint8_t cellX, uint8_t cellY)
{
	if (!inBounds(cellX, cellY))
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