#pragma once

#include "../symbol.hpp"
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>

namespace xxr { namespace xcsr_impl { namespace ubr
{

    // The standard symbol for XCSR_UB (without "don't care")
    template <typename T>
    class Symbol : public xcsr_impl::AbstractSymbol<T>
    {
    public:
        T p;
        T q;

        // Constructor
        constexpr explicit Symbol(T value) : p(value), q(value) {}
        constexpr Symbol(T p, T q) : p(p), q(q) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            std::ostringstream stream;
            stream << std::setprecision(3) << p << ';' << q << ' ';
            return stream.str();
        }

        friend bool operator== (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.p == rhs.p && lhs.q == rhs.q;
        }

        friend bool operator!= (const Symbol & lhs, const Symbol & rhs)
        {
            return lhs.p != rhs.p || lhs.q != rhs.q;
        }

        virtual Symbol & operator= (const Symbol & obj)
        {
            p = obj.p;
            q = obj.q;
            return *this;
        }

        virtual bool isDontCare() const override
        {
            assert(false);

            return false;
        }

        virtual T lower() const noexcept override
        {
            return std::min(p, q);
        }

        virtual T upper() const noexcept override
        {
            return std::max(p, q);
        }
    };

}}}
