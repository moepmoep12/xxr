#pragma once

#include "../XCS/symbol.h"
#include <string>

// The standard symbol for XCSR (without "don't care")
template <typename T>
class CSSymbol : public AbstractSymbol<CSSymbol<T>>
{
private:
    T m_c;
    T m_s;

public:
    CSSymbol(T c, T s) : m_c(c), m_s(s) {}

    std::string toString() const override
    {
        return std::to_string(m_c) + "(" + std::to_string(m_s) + ") ";
    }

    friend bool operator== (const CSSymbol & lhs, const CSSymbol & rhs)
    {
        return lhs.m_c == rhs.m_c && lhs.m_s == rhs.m_s;
    }

    friend bool operator!= (const CSSymbol & lhs, const CSSymbol & rhs)
    {
        return lhs.m_c != rhs.m_c || lhs.m_s != rhs.m_s;
    }

    CSSymbol & operator= (const CSSymbol & obj)
    {
        m_c = obj.m_c;
        m_s = obj.m_s;
        return *this;
    }

    bool contains(const Symbol<T> & symbol) const override
    {
        return (m_value == symbol.m_value && isDontCare() == symbol.isDontCare());
    }
};