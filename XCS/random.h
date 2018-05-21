#pragma once

#include <random>

class Random
{
private:
	static std::random_device m_device;

	static auto && engine()
	{
		static std::mt19937 engine(m_device());
		return engine;
	}

public:
	static double nextDouble(double max = 1.0, double min = 0.0)
	{
		std::uniform_real_distribution<double> dist(min, max);
		return dist(engine);
	}
};