#pragma once

#include <vector>
#include <cstddef>
#include <cassert>
#include <unordered_set>

namespace xxr
{

    template <typename T, typename Action>
    class AbstractEnvironment
    {
    protected:
        // Constructor with available action choices
        AbstractEnvironment(const std::unordered_set<Action> & availableActions) : availableActions(availableActions)
        {
        }

    public:
        // Available action choices
        const std::unordered_set<Action> availableActions;

        // Disable default constructor to force derived classes to set available action choices
        AbstractEnvironment() = delete;

        // Destructor
        virtual ~AbstractEnvironment() = default;

        // Returns current situation
        virtual std::vector<T> situation() const = 0;

        // Executes action (and update situation) and returns reward
        virtual double executeAction(Action action) = 0;

        // Returns true if the problem was solved by the previous action
        // (always true for a single-step problem after the first action execution)
        virtual bool isEndOfProblem() const = 0;
    };

}
