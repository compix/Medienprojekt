#pragma once
#include <vector>
#include <ecstasy/core/Entity.h>
#include "Components/TransformComponent.h"
#include <array>
#include <algorithm>
#include <assert.h>

using std::vector;

typedef std::vector<Entity *> EntityCollection; // A collection is needed because more than one entity can be on the same cell
typedef EntityCollection** EntityGrid;

class EntityLayer
{
public:
	class IOnChangeListener
	{
	public:
		virtual ~IOnChangeListener() {};
		virtual void onEntityAdded(Entity *entity) = 0;
		virtual void onEntityRemoved(Entity *entity) = 0;
	};
public:
	/**
     * @param int value Value of the layer for a correct drawing order.
     */
	EntityLayer(int width, int height, int value);
	~EntityLayer();

	inline EntityGrid getEntityGrid() const { return m_grid; };

	template<class T>
	void sort(T comparator, int cellX, int cellY);

	void add(Entity *entity, int cellX, int cellY);
	void remove(Entity *entity, int cellX, int cellY);

	inline int getValue() const { return m_value; }

	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }

	inline EntityCollection& get(int cellX, int cellY) { return m_grid[cellX][cellY]; };

	inline void listen(IOnChangeListener* listener) { m_changeListeners.push_back(listener); }

	void stopListening(IOnChangeListener* listener);
private:
	EntityGrid m_grid;

	int m_width, m_height;
	int m_value; // The value of the layer for a correct drawing order.

	std::vector<IOnChangeListener*> m_changeListeners;
};

template <class T>
void EntityLayer::sort(T comparator, int cellX, int cellY)
{
	std::sort(m_grid[cellX][cellY].begin(), m_grid[cellX][cellY].end(), comparator);
}

/**
 * Used for rendering to create depth. Entities farther away are rendered first.
 */
struct DepthComparator
{
	bool operator()(const Entity *e1, const Entity *e2)
	{
		auto t1 = e1->get<TransformComponent>();
		auto t2 = e2->get<TransformComponent>();
		assert(t1 && t2);

		return (t1->y == t2->y) ? (t1->x < t2->x) : (t1->y < t2->y);
	}
};

