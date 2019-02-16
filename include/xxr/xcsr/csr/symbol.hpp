#pragma once

#include "../../xcs/symbol.hpp"
#include <sstream>
#include <iomanip>
#include <string>

namespace xxr { namespace xcsr_impl { namespace csr
{

    // The standard symbol for XCSR (without "don't care")
    template <typename T>
    class Symbol : public xcs_impl::AbstractSymbol<T>
    {
    public:
        T center;
        T spread;

        // Constructor
        constexpr Symbol(T c, T s = 0.0) : center(c), spread(s) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            std::ostringstream stream;
            stream << std::setprecision(3) << center << ";" << spread << " ";
            return stream.str();
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
        virtual bool matches(T value) const noexcept override
        {
            return (center - spread) <= value && value < (center + spread);
        }

        virtual void generalize() override
        {
            assert(false);
        }
    };

}}}
