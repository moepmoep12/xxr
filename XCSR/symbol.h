#pragma once

#include "../XCS/symbol.h"
#include <string>

namespace XCSR
{

    // The standard symbol for XCSR (without "don't care")
    template <typename T>
    class Symbol : public XCS::AbstractSymbol<T>
    {
    private:
        T m_center;
        T m_spread;

    public:
        // Constructor
        Symbol(T c, T s = 0.0) : m_center(c), m_spread(s) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            return std::to_string(m_center) + "(" + std::to_string(m_spread) + ") ";
        }

        friend bool operator== (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.m_center == rhs.m_center && lhs.m_spread == rhs.m_spread;
        }

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.m_center != rhs.m_center || lhs.m_spread != rhs.m_spread;
        }

        virtual Symbol & operator= (const Symbol & obj)
        {
            m_center = obj.m_center;
            m_spread = obj.m_spread;
            return *this;
        }

        // DOES MATCH
        virtual bool matches(T value) const override
        {
            return (m_center - m_spread) <= value && value < (m_center + m_spread);
        }
    };

}