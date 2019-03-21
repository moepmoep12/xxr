#pragma once

#include "../symbol.hpp"
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>

namespace xxr { namespace xcsr_impl { namespace obr
{

    // The standard symbol for XCSR_LU (without "don't care")
    template <typename T>
    class Symbol : public xcsr_impl::AbstractSymbol<T>
    {
    public:
        T l;
        T u;

        // Constructor
        constexpr explicit Symbol(T value) : l(value), u(value) {}
        constexpr Symbol(T l, T u) : l(l), u(u) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            std::ostringstream stream;
            stream << std::setprecision(3) << l << ';' << u << ' ';
            return stream.str();
        }

        friend bool operator== (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.l == rhs.l && lhs.u == rhs.u;
        }

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.l != rhs.l || lhs.u != rhs.u;
        }

        virtual Symbol & operator= (const Symbol & obj)
        {
            l = obj.l;
            u = obj.u;
            return *this;
        }

        virtual T lower() const noexcept override
        {
            return l;
        }

        virtual T upper() const noexcept override
        {
            return u;
        }
    };

}}}
