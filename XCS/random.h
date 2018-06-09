#pragma once

#include <random>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <cstddef>

namespace XCS
{

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
        static auto chooseFrom(const std::vector<T> & container)
        {
            auto size = container.size();

            assert(size > 0);

            std::uniform_int_distribution<decltype(size)> dist(0, size - 1);
            return *(std::begin(container) + dist(engine()));
        }

        template <typename T>
        static auto chooseFrom(const std::unordered_set<T> & container)
        {
            std::vector<T> vec(container.size());
            vec.insert(vec.end(), container.begin(), container.end());
            return chooseFrom(vec);
        }

        template <typename T>
        static std::size_t spinRouletteWheel(const std::vector<T> & container)
        {
            // Prepare a roulette wheel by the weights
            T sum = 0;
            std::vector<T> rouletteWheel;
            rouletteWheel.reserve(container.size());
            for (auto && value : container)
            {
                sum += value;
                rouletteWheel.push_back(sum);
            }

            assert(sum > 0);

            // Spin the roulette wheel
            auto it = std::lower_bound(std::begin(rouletteWheel), std::end(rouletteWheel), nextDouble<T>(0, sum));

            // Returns index of selected item
            return std::distance(std::begin(rouletteWheel), it);
        }
    };

    std::random_device Random::m_device;

}