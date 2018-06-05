#pragma once

#include "../XCS/symbol.h"
#include <string>

namespace XCSR
{

    // The standard symbol for XCSR (without "don't care")
    template <typename T>
    class Symbol : public XCS::AbstractSymbol<T>
    {
    public:
        T center;
        T spread;

        // Constructor
        Symbol(T c, T s = 0.0) : center(c), spread(s) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            return std::to_string(center) + "(" + std::to_string(spread) + ") ";
        }

        friend bool operator== (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.center == rhs.center && lhs.spread == rhs.spread;
        }

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.center != rhs.center || lhs.spread != rhs.spread;
        }

        virtual Symbol & operator= (const Symbol & obj)
        {
            center = obj.center;
            spread = obj.spread;
            return *this;
        }

        virtual bool isDontCare() const override
        {
            assert(false);

            return false;
        }

        // DOES MATCH
        virtual bool matches(T value) const override
        {
            return (center - spread) <= value && value < (center + spread);
        }

        virtual void generalize() override
        {
            assert(false);
        }
    };

}