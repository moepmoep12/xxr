#pragma once

#include "../XCS/symbol.h"
#include <string>

class CSSymbol : public AbstractSymbol<CSSymbol>
{
private:
    double m_c;
    double m_s;

public:
    CSSymbol(double c, double s) : m_c(c), m_s(s) {}

    bool isDontCare() const override
    {
        return false;
    }

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
};