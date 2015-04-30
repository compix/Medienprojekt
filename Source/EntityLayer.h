#pragma once
#include <vector>
#include <entityx/entityx.h>
#include "Components/TransformComponent.h"
using namespace std;
using namespace entityx;

class EntityLayer
{
public:
	/**
	int value: Value of the layer for a correct drawing order.
	bool _static: To indicate whether the entities in this layer are static or dynamic.
	*/
	EntityLayer(int value, bool _static);

	inline const vector<Entity>& getEntities() const { return m_entities; };

	template<class T>
	void sort(T comparator);

	void add(Entity entity);
	void remove(Entity entity);

	/**
	Indicates whether the entities in this layer are static or dynamic.
	A static layer rarely needs to be resorted.
	*/
	inline bool isStatic() const { return m_static; };
	inline int getValue() const { return m_value; }
private:
	int m_value; // The value of the layer for a correct drawing order.
	bool m_static;
	vector<Entity> m_entities;
};

template<class T>
void EntityLayer::sort(T comparator)
{
	std::sort(m_entities.begin(), m_entities.end(), comparator);
}

/**
Used for rendering to create depth. Entities farther away are rendered first.
*/
struct DepthComparator
{
	bool operator()(Entity& e1, Entity& e2)
	{
		assert(e1.has_component<TransformComponent>() && e2.has_component<TransformComponent>());
		ComponentHandle<TransformComponent> t1 = e1.component<TransformComponent>();
		ComponentHandle<TransformComponent> t2 = e2.component<TransformComponent>();

		return (t1->y == t2->y) ? (t1->x < t2->x) : (t1->y < t2->y);
	}
};

