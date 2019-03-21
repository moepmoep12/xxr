#pragma once
#include <vector>

namespace xxr
{

    template <typename T, typename Action>
    struct Dataset
    {
        std::vector<std::vector<T>> situations;
        std::vector<Action> actions;

        // Normalize data between 0.0 to 1.0 and return min/max value
        std::pair<T, T> normalize()
        {
            T min;
            T max;
            for (auto && situation : situations)
            {
                for (auto && value : situation)
                {
                    if (min > value) min = value;
                    if (max < value) max = value;
                }
            }

            for (auto && situation : situations)
            {
                for (auto && value : situation)
                {
                    value = (value - min) / (max - min);
                }
            }

            return { min, max };
        }

        // Denormalize data by min/max value
        void denormalize(T min, T max)
        {
            if (min != 0.0 || max != 1.0)
            {
                for (auto && situation : situations)
                {
                    for (auto && value : situation)
                    {
                        value = value * (max - min) + min;
                    }
                }
            }
        }

        void denormalize(const std::pair<T, T> & minmax)
        {
            denormalize(minmax.first, minmax.second);
        }
    };

}
