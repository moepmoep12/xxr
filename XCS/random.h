#pragma once

#include <random>
#include <vector>

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
	template <typename T = double>
	static T nextDouble(T min = 0.0, T max = 1.0)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(engine());
	}

	template <typename T = int>
	static T nextInt(T min, T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(engine());
	}

	template <typename T>
	static T chooseFrom(const std::vector<T> vec)
	{
		auto size = std::size(vec);
		std::uniform_int_distribution<decltype(size)> dist(0, size - 1);
		return vec.at(dist(engine()));
	}
};

std::random_device Random::m_device;