#pragma once

#include "../xcs/symbol.hpp"

namespace xxr { namespace xcsr_impl
{

    // The base class of XCSR symbol (without "don't care")
    template <typename T>
    class AbstractSymbol : public xcs_impl::AbstractSymbol<T>
    {
    public:
        // Destructor
        virtual ~AbstractSymbol() = default;

        // Lower/Upper bound of the interval
        virtual T lower() const noexcept = 0;
        virtual T upper() const noexcept = 0;

        virtual bool isDontCare() const override
        {
            assert(false);

            return false;
        }

        virtual void setDontCare() override
        {
            assert(false);
        }

        // DOES MATCH
        virtual bool matches(T value) const noexcept override
        {
            return lower() <= value && value < upper();
        }
    };

}}
