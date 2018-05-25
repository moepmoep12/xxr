#pragma once

#include <cstddef>
#include <cassert>

#include "symbol.h"
#include "state.h"

template <class S = BinarySymbol, typename Action = int>
class Environment
{
public:
    // Returns condition
    virtual State<S> situation() const = 0;

    // Returns reward for action execution
    virtual double executeAction(Action action) = 0;
};

class MultiplexerEnvironment : public Environment<BinarySymbol, bool>
{
private:
    const size_t m_totalLength;
    const size_t m_addressBitLength;
    const size_t m_registerBitLength;
    State<BinarySymbol> m_situation;

    // Convert total length to address bit length
    static constexpr size_t addressBitLength(size_t l, size_t c)
    {
        return (l == 0) ? c : addressBitLength(l >> 1, c + 1);
    }

    static State<BinarySymbol> randomSituation(size_t totalLength)
    {
        std::vector<BinarySymbol> symbols;
        for (size_t i = 0; i < totalLength; ++i)
        {
            symbols.push_back(BinarySymbol(Random::nextInt(0, 1)));
        }
        return State<BinarySymbol>(symbols);
    }

public:
    using S = BinarySymbol;
    using Action = bool;

    explicit MultiplexerEnvironment(size_t length) :
        m_totalLength(length),
        m_addressBitLength(addressBitLength(length, 0)),
        m_registerBitLength(length - m_addressBitLength),
        m_situation(randomSituation(length))
    {
        // Total length must be n + 2^n (n > 0)
        assert(m_totalLength == (m_addressBitLength - 1 + ((size_t)1 << (m_addressBitLength - 1))));
    }

    State<BinarySymbol> situation() const override
    {
        return m_situation;
    }

    double executeAction(Action action) override
    {
        double reward = (action == getAnswer(m_situation)) ? 1.0 : 0.0;

        // Update situation
        m_situation = randomSituation(m_totalLength);

        return reward;
    }

    // Returns answer for situation
    Action getAnswer(const State<BinarySymbol> & situation) const
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
