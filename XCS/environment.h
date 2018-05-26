#pragma once

#include <unordered_set>
#include <cstddef>
#include <cassert>

#include "symbol.h"
#include "situation.h"
#include "xcs.h"

template <class Symbol, typename Action>
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
    virtual Situation<Symbol> situation() const = 0;

    // Executes action (and update situation) and returns reward
    virtual double executeAction(Action action) = 0;
};

class MultiplexerEnvironment : public AbstractEnvironment<BinarySymbol, bool>
{
private:
    const size_t m_totalLength;
    const size_t m_addressBitLength;
    const size_t m_registerBitLength;
    Situation<BinarySymbol> m_situation;

    // Get address bit length from total length
    static constexpr size_t addressBitLength(size_t l, size_t c)
    {
        return (l == 0) ? c - 1 : addressBitLength(l >> 1, c + 1);
    }

    static Situation<BinarySymbol> randomSituation(size_t totalLength)
    {
        std::vector<BinarySymbol> symbols;
        for (size_t i = 0; i < totalLength; ++i)
        {
            symbols.push_back(BinarySymbol(Random::nextInt(0, 1)));
        }
        return Situation<BinarySymbol>(symbols);
    }

public:
    explicit MultiplexerEnvironment(size_t length) :
        AbstractEnvironment({ 0, 1 }),
        m_totalLength(length),
        m_addressBitLength(addressBitLength(length, 0)),
        m_registerBitLength(length - m_addressBitLength),
        m_situation(randomSituation(length))
    {
        // Total length must be n + 2^n (n > 0)
        assert(m_totalLength == (m_addressBitLength + ((size_t)1 << m_addressBitLength)));
    }

    Situation<BinarySymbol> situation() const override
    {
        return m_situation;
    }

    double executeAction(bool action) override
    {
        double reward = (action == getAnswer(m_situation)) ? 1.0 : 0.0;

        // Update situation
        m_situation = randomSituation(m_totalLength);

        return reward;
    }

    // Returns answer to situation
    bool getAnswer(const Situation<BinarySymbol> & situation) const
    {
        size_t address = 0;
        for(size_t i = 0; i < m_addressBitLength; ++i)
        {
            if (situation[i].toChar() == '1')
            {
                address += (size_t)1 << (m_addressBitLength - i - 1);
            }
        }

        return situation[m_addressBitLength + address].toChar() == '1';
    }
};
