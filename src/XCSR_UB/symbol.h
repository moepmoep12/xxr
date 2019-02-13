#pragma once

#include "../XCS/symbol.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>

namespace XCSR_UB
{

    // The standard symbol for XCSR_UB (without "don't care")
    template <typename T>
    class Symbol : public XCS::AbstractSymbol<T>
    {
    public:
        T p;
        T q;

        // Constructor
        constexpr Symbol(T value) : p(value), q(value) {}
        constexpr Symbol(T p, T q) : p(p), q(q) {}

        // Destructor
        virtual ~Symbol() = default;

        virtual std::string toString() const override
        {
            std::ostringstream stream;
            stream << std::setprecision(3) << p << ";" << q << " ";
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

        // DOES MATCH
        virtual bool matches(T value) const noexcept override
        {
            return lower() - std::numeric_limits<double>::epsilon() <= value && value < upper() + std::numeric_limits<double>::epsilon();
        }

        virtual void generalize() override
        {
            assert(false);
        }

        virtual double upper() const
        {
            return std::max(p, q);
        }

        virtual double lower() const
        {
            return std::min(p, q);
        }
    };

}