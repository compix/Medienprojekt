#pragma once
#include <vector>
#include <entityx/entityx.h>
#include "Components/TransformComponent.h"
#include <array>

using std::vector;
using entityx::Entity;
using entityx::ComponentHandle;
using entityx::EntityManager;

typedef std::vector<Entity> EntityCollection; // A collection is needed because more than one entity can be on the same cell
typedef EntityCollection** EntityGrid;

class EntityLayer
{
public:
	class IOnChangeListener
	{
	public:
		virtual ~IOnChangeListener() {};
		virtual void onEntityAdded(entityx::Entity& entity) = 0;
		virtual void onEntityRemoved(entityx::Entity& entity) = 0;
	};
public:
	/**
     * @param int value Value of the layer for a correct drawing order.
     */
	EntityLayer(uint8_t width, uint8_t height, int value);
	~EntityLayer();

	void reset();

	inline EntityGrid getEntityGrid() const { return m_grid; }

	template<class T>
	void sort(T comparator, uint8_t cellX, uint8_t cellY);

	void add(Entity entity, uint8_t cellX, uint8_t cellY);
	void remove(Entity entity, uint8_t cellX, uint8_t cellY);

	inline int getValue() const { return m_value; }

	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }

	inline EntityCollection& get(uint8_t cellX, uint8_t cellY)
	{
		assert(inBounds(cellX, cellY));
		return m_grid[cellX][cellY];
	};

	inline bool inBounds(uint8_t cellX, uint8_t cellY) { return cellX < m_width && cellY < m_height; }

	inline void listen(IOnChangeListener* listener) { m_changeListeners.push_back(listener); }

	void stopListening(IOnChangeListener* listener);
private:
	EntityGrid m_grid;

	int m_width, m_height;
	int m_value; // The value of the layer for a correct drawing order.

	std::vector<IOnChangeListener*> m_changeListeners;
};

template <class T>
void EntityLayer::sort(T comparator, uint8_t cellX, uint8_t cellY)
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

