#pragma once
#include <vector>
#include <entityx/entityx.h>
#include "Components/TransformComponent.h"
#include <array>

typedef std::vector<Entity> EntityCollection; // A collection is needed because more than one entity can be on the same cell
typedef EntityCollection** EntityGrid;
using std::vector;
using entityx::Entity;
using entityx::ComponentHandle;
using entityx::EntityManager;

class EntityLayer
{
public:
	/**
     * @param int value Value of the layer for a correct drawing order.
     */
	EntityLayer(int width, int height, int value);
	~EntityLayer();

	inline EntityGrid getEntityGrid() const { return m_grid; };

	template<class T>
	void sort(T comparator, int cellX, int cellY);

	void add(Entity entity, int cellX, int cellY);
	void remove(Entity entity, int cellX, int cellY);

	inline int getValue() const { return m_value; }

	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }

	inline EntityCollection& get(int cellX, int cellY) { return m_grid[cellX][cellY]; };
private:
	EntityGrid m_grid;

	int m_width, m_height;
	int m_value; // The value of the layer for a correct drawing order.
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
	bool operator()(const Entity& e1, const Entity& e2)
	{
		assert(e1.has_component<TransformComponent>() && e2.has_component<TransformComponent>());
		const ComponentHandle<const TransformComponent, const EntityManager> t1 = e1.component<const TransformComponent, const EntityManager>();
		const ComponentHandle<const TransformComponent, const EntityManager> t2 = e2.component<const TransformComponent, const EntityManager>();

		return (t1->y == t2->y) ? (t1->x < t2->x) : (t1->y < t2->y);
	}
};

