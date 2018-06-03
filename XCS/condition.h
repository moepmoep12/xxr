#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <cassert>
#include <iterator>

#include "symbol.h"
#include "random.h"

template <typename T, class Symbol = Symbol<T>>
class Condition
{
private:
    std::vector<Symbol> m_symbols;

public:
    Condition() = default;

    Condition(const std::vector<Symbol> & symbols) : m_symbols(symbols) {}

    Condition(const std::vector<T> & symbols) : m_symbols(symbols.begin(), symbols.end()) {}

    Condition(const std::string & symbols)
    {
        for (const char symbol : symbols)
        {
            m_symbols.push_back(symbol);
        }
    }

    virtual std::string toString() const
    {
        std::string str;
        for (auto && symbol : m_symbols)
        {
            str += symbol.toString();
        }
        return str;
    }

    virtual Symbol & operator[] (std::size_t idx)
    {
        return m_symbols[idx];
    }

    virtual const Symbol & at(std::size_t idx) const
    {
        return m_symbols.at(idx);
    }

    friend std::ostream & operator<< (std::ostream & os, const Condition & obj)
    {
        return os << obj.toString();
    }

    friend bool operator== (const Condition & lhs, const Condition & rhs)
    {
        return lhs.m_symbols == rhs.m_symbols;
    }

    friend bool operator!= (const Condition & lhs, const Condition & rhs)
    {
        return lhs.m_symbols != rhs.m_symbols;
    }

    // DOES MATCH
    virtual bool matches(const std::vector<T> & situation) const
    {
        assert(m_symbols.size() == situation.size());

        for (std::size_t i = 0; i < m_symbols.size(); ++i)
        {
            if (!m_symbols[i].matches(situation[i]))
            {
                return false;
            }
        }

        return true;
    }

    virtual bool empty() const
    {
        return m_symbols.empty();
    }

    virtual std::size_t size() const
    {
        return m_symbols.size();
    }

    virtual void randomGeneralize(double generalizeProbability)
    {
        for (auto && symbol : m_symbols)
        {
            if (Random::nextDouble() < generalizeProbability)
            {
                symbol.generalize();
            }
        }
    }

    virtual std::size_t dontCareCount() const
    {
        std::size_t count = 0;
        for (auto && symbol : m_symbols)
        {
            if (symbol.isDontCare())
            {
                count++;
            }
        }

        return count;
    }
};