#pragma once

template <class S = BinarySymbol, typename Action = int>
class Environment
{
public:
    // Returns condition
    virtual State<S> getSituation() const = 0;

    // Returns reward for action execution
    virtual double executeAction(Action action) = 0;
};