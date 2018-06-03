#pragma once

#include <unordered_set>
#include <cstddef>
#include <cassert>

#include "symbol.h"
#include "condition.h"
#include "xcs.h"

template <typename T, typename Action, class Symbol = Symbol<T>>
class AbstractEnvironment
{
protected:
    // Constructor with available action choices
    AbstractEnvironment(const std::unordered_set<Action> & actionChoices) : actionChoices(actionChoices) {}

public:
    // Available action choices
    const std::unordered_set<Action> actionChoices;

    // Disable default constructor to force derived classes to set available action choices
    AbstractEnvironment() = delete;

    // Destructor
    virtual ~AbstractEnvironment() = default;

    // Returns current situation
    virtual Condition<T> situation() const = 0;

    // Executes action (and update situation) and returns reward
    virtual double executeAction(Action action) = 0;

    // Returns true if the problem was solved by the previous action
    // (always true for a single-step problem after the first action execution)
    virtual bool isEndOfProblem() const = 0;
};

class MultiplexerEnvironment : public AbstractEnvironment<bool, bool>
{
private:
    const size_t m_totalLength;
    const size_t m_addressBitLength;
    const size_t m_registerBitLength;
    Condition<bool> m_situation;
    bool m_isEndOfProblem;

    // Get address bit length from total length
    static constexpr size_t addressBitLength(size_t l, size_t c)
    {
        return (l == 0) ? c - 1 : addressBitLength(l >> 1, c + 1);
    }

    static Condition<bool> randomSituation(size_t totalLength)
    {
        std::vector<Symbol<bool>> symbols;
        for (size_t i = 0; i < totalLength; ++i)
        {
            symbols.push_back(Symbol<bool>(Random::nextInt(0, 1)));
        }
        return Condition<bool>(symbols);
    }

public:
    explicit MultiplexerEnvironment(size_t length) :
        AbstractEnvironment({ 0, 1 }),
        m_totalLength(length),
        m_addressBitLength(addressBitLength(length, 0)),
        m_registerBitLength(length - m_addressBitLength),
        m_situation(randomSituation(length)),
        m_isEndOfProblem(false)
    {
        // Total length must be n + 2^n (n > 0)
        assert(m_totalLength == (m_addressBitLength + ((size_t)1 << m_addressBitLength)));
    }

    Condition<bool> situation() const override
    {
        return m_situation;
    }

    double executeAction(bool action) override
    {
        double reward = (action == getAnswer(m_situation)) ? 1.0 : 0.0;

        // Update situation
        m_situation = randomSituation(m_totalLength);

        // Single-step problem
        m_isEndOfProblem = true;

        return reward;
    }

    bool isEndOfProblem() const override
    {
        return m_isEndOfProblem;
    }

    // Returns answer to situation
    bool getAnswer(const Condition<bool> & situation) const
    {
        size_t address = 0;
        for (size_t i = 0; i < m_addressBitLength; ++i)
        {
            if (situation.at(i).equals(true))
            {
                address += (size_t)1 << (m_addressBitLength - i - 1);
            }
        }

        return situation.at(m_addressBitLength + address).equals(true);
    }
};
