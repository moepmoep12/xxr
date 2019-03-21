#pragma once
#include <string>
#include <unordered_set>

#include "dataset_environment.hpp"
#include "xxr/helper/csv.hpp"

namespace xxr
{

    template <typename T, typename Action>
    class
    [[deprecated("please use CSV::readDataset() helper function and DatasetEnvironment class")]]
    CSVEnvironment : public DatasetEnvironment<T, Action>
    {
    public:
        CSVEnvironment(std::istream & is, const std::unordered_set<Action> & availableActions, bool chooseRandom = true)
            : DatasetEnvironment<T, Action>(CSV::readDataset<T, Action>(is), availableActions, chooseRandom)
        {
        }

        CSVEnvironment(const std::string & filename, const std::unordered_set<Action> & availableActions, bool chooseRandom = true)
            : DatasetEnvironment<T, Action>(CSV::readDataset<T, Action>(filename), availableActions, chooseRandom)
        {
        }

        virtual ~CSVEnvironment() = default;

        using DatasetEnvironment<T, Action>::situation;
        using DatasetEnvironment<T, Action>::executeAction;
        using DatasetEnvironment<T, Action>::isEndOfProblem;
        using DatasetEnvironment<T, Action>::getAnswer;
    };

}
