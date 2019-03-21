#pragma once

#include "../symbol.hpp"
#include <sstream>
#include <iomanip>
#include <string>

namespace xxr { namespace xcsr_impl { namespace csr
{

    // The standard symbol for XCSR (without "don't care")
    template <typename T>
    class Symbol : public xcsr_impl::AbstractSymbol<T>
    {
    public:
        T center;
        T spread;

        // Constructor
        constexpr explicit Symbol(T c, T s = 0.0) : center(c), spread(s) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            std::ostringstream stream;
            stream << std::setprecision(3) << center << ';' << spread << ' ';
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

        virtual T lower() const noexcept override
        {
            return center - spread;
        }

        virtual T upper() const noexcept override
        {
            return center + spread;
        }
    };

}}}
