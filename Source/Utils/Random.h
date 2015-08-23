#pragma once

#include <random>

/**
Uses the std::mt19937 random number generator and uniform distributions.
*/
class Random
{
public:
	static void randomize();

	static inline void seed(unsigned int seed) { m_rng.seed(seed); }

	/**
	Returns a random int between start (included) and end(included).
	*/
	static int getInt(int start, int end);


	/**
	Returns a random float between start (included) and end(included).
	*/
	static float getFloat(float start, float end);

	inline static std::mt19937& RNG() { return m_rng; }
private:
	static std::mt19937 m_rng;
};


