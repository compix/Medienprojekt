#pragma once
#include <memory>

#ifndef HAS_STD_MAKE_UNIQUE
namespace std
{
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}
#else
	#include <memory>
#endif