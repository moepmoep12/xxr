#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <cassert>

#include "symbol.h"
#include "random.h"

// Set isCacheEnabled to false if the number of don't care symbols can change randomly
template <class Symbol, bool isCacheEnabled = true>
class Situation
{
private:
    std::vector<Symbol> m_symbols;

    mutable size_t dontCareCountCache;
    mutable bool dontCareCountCacheExists;

public:
    static constexpr bool isCacheEnabled = isCacheEnabled;

    Situation(const std::vector<Symbol> & symbols) : m_symbols(symbols) {}

    Situation(const std::string & symbols)
    {
        for (const char symbol : symbols)
        {
            m_symbols.push_back(symbol);
        }
    }

    std::string toString() const
    {
        std::string str;
        for (auto && symbol : m_symbols)
        {
            str += symbol.toString();
        }
        return str;
    }

    Symbol & operator[] (size_t idx)
    {
        return m_symbols[idx];
    }

    const Symbol & operator[] (size_t idx) const
    {
        return m_symbols[idx];
    }

    friend std::ostream & operator<< (std::ostream & os, const Situation & obj)
    {
        return os << obj.toString();
    }

    friend bool operator== (const Situation & lhs, const Situation & rhs)
    {
        return lhs.m_symbols == rhs.m_symbols;
    }

    friend bool operator!= (const Situation & lhs, const Situation & rhs)
    {
        return lhs.m_symbols != rhs.m_symbols;
    }

    bool contains(const Situation & state) const
    {
        assert(std::size(m_symbols) == std::size(state.m_symbols));

        for (size_t i = 0; i < std::size(m_symbols); ++i)
        {
            if (!m_symbols[i].contains(state.m_symbols[i]))
            {
                return false;
            }
        }

        return true;
    }

    size_t size() const
    {
        return std::size(m_symbols);
    }

    void randomGeneralize(double generalizeProbability)
    {
        for (auto && symbol : m_symbols)
        {
            if (Random::nextDouble() < generalizeProbability)
            {
                symbol.generalize();
            }
        }

        dontCareCountCacheExists = false;
    }

    size_t dontCareCount() const
    {
        // Cache the result in case of a huge number of symbols
        if (!isCacheEnabled || !dontCareCountCacheExists)
        {
            dontCareCountCache = 0;
            for (auto && symbol : m_symbols)
            {
                if (symbol.isDontCare())
                {
                    dontCareCountCache++;
                }
            }
        }

        return dontCareCountCache;
    }
};