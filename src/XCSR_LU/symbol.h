#pragma once

#include "../XCS/symbol.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>

namespace XCSR_LU
{

    // The standard symbol for XCSR_LU (without "don't care")
    template <typename T>
    class Symbol : public XCS::AbstractSymbol<T>
    {
    public:
        T lower;
        T upper;

        // Constructor
        constexpr Symbol(T value) : lower(value), upper(value) {}
        constexpr Symbol(T l, T u) : lower(l), upper(u) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            std::ostringstream stream;
            stream << std::setprecision(3) << lower << ";" << upper << " ";
            return stream.str();
        }

        friend bool operator== (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.lower == rhs.lower && lhs.upper == rhs.upper;
        }

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.lower != rhs.lower || lhs.upper != rhs.upper;
        }

        virtual Symbol & operator= (const Symbol & obj)
        {
            lower = obj.lower;
            upper = obj.upper;
            return *this;
        }

        virtual bool isDontCare() const override
        {
            assert(false);

            return false;
        }

        // DOES MATCH
        virtual bool matches(T value) const noexcept override
        {
            return lower - std::numeric_limits<double>::epsilon() <= value && value < upper + std::numeric_limits<double>::epsilon();
        }

        virtual void generalize() override
        {
            assert(false);
        }
    };

}