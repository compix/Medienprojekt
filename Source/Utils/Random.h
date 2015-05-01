#pragma once

#include <random>

/**
Uses the std::mt19937 random number generator and uniform distributions.
*/
class Random
{
public:
	static void randomize()
	{
		std::random_device rd;
		m_rng.seed(rd());
	}

	static void seed(unsigned int seed)
	{
		m_rng.seed(seed);
	}

	/**
	Returns a random int between start (included) and end(included).
	*/
	static int getInt(int start, int end)
	{
		std::uniform_int_distribution<int> distribution(start, end);
		return distribution(m_rng);
	}

	/**
	Returns a random float between start (included) and end(included).
	*/
	static float getFloat(float start, float end)
	{
		std::uniform_real_distribution<float> distribution(start, std::nextafter(end, FLT_MAX));
		return distribution(m_rng);
	}

private:
	static std::mt19937 m_rng;
};

std::mt19937 Random::m_rng;
