#pragma once
#include <vector>

namespace xxr
{

    template <typename T, typename Action>
    struct Dataset
    {
        std::vector<std::vector<T>> situations;
        std::vector<Action> actions;
    };

}
