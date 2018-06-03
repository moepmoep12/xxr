#pragma once

#include "../XCS/symbol.h"
#include <string>

namespace xcsr
{

    // The standard symbol for XCSR (without "don't care")
    template <typename T>
    class CSSymbol : public xcs::AbstractSymbol<T>
    {
    private:
        T m_center;
        T m_spread;

    public:
        CSSymbol(T c, T s) : m_center(c), m_spread(s) {}

        virtual std::string toString() const override
        {
            return std::to_string(m_center) + "(" + std::to_string(m_spread) + ") ";
        }

        friend bool operator== (const CSSymbol & lhs, const CSSymbol & rhs)
        {
            return lhs.m_center == rhs.m_center && lhs.m_spread == rhs.m_spread;
        }

        friend bool operator!= (const CSSymbol & lhs, const CSSymbol & rhs)
        {
            return lhs.m_center != rhs.m_center || lhs.m_spread != rhs.m_spread;
        }

        virtual CSSymbol & operator= (const CSSymbol & obj)
        {
            m_center = obj.m_center;
            m_spread = obj.m_spread;
            return *this;
        }

        virtual bool matches(const T & value) const override
        {
            return (m_center - m_spread) <= value && value < (m_center + m_spread);
        }
    };

}