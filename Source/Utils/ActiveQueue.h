#pragma once
#include <algorithm>
#include <vector>

/**
* If an added element is already in the queue then it is removed first and then added again at the correct position.
* Equality is checked with == operator, not with the specified Compare functor.
*/
template<class T, class Compare = std::less<T>>
class ActiveQueue
{
public:
	void put(T value)
	{
		remove(value);
		auto it = std::lower_bound(m_values.begin(), m_values.end(), value, Compare());
		m_values.insert(it, value);
	}

	void remove(T value)
	{
		if (m_values.empty())
			return;

		for (auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			if ((*it) == value)
			{
				m_values.erase(it);
				break;
			}
		}
	}

	inline bool empty() const { return m_values.empty(); }

	// Skill set is never empty. If the entity has no skills then SkillType::NONE is returned.
	inline const T& top() { return *m_values.begin(); }

private:
	std::vector<T> m_values;
};