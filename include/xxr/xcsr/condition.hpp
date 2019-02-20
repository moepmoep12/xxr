#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstddef>
#include <cassert>

#include "../xcs/condition.hpp"

namespace xxr { namespace xcsr_impl
{

    template <class Symbol>
    class Condition : public xcs_impl::Condition<Symbol>
    {
    public:
        using typename xcs_impl::Condition<Symbol>::type;
        using typename xcs_impl::Condition<Symbol>::SymbolType;

    protected:
        using xcs_impl::Condition<Symbol>::m_symbols;

    public:
        // Constructor
        Condition() = default;

        Condition(const std::vector<Symbol> & symbols) : xcs_impl::Condition<Symbol>(symbols) {}

        Condition(const std::vector<type> & symbols) : xcs_impl::Condition<Symbol>(symbols) {}

        explicit Condition(const std::string & symbols)
        {
            std::istringstream iss(symbols);
            std::string symbol;
            while (std::getline(iss, symbol, ' '))
            {
                std::size_t semicolonIdx = symbol.find(';');

                // Make sure the string has at least one semicolon
                assert(semicolonIdx != std::string::npos);

                m_symbols.emplace_back(
                    std::stod(symbol.substr(0, semicolonIdx)),
                    std::stod(symbol.substr(semicolonIdx + 1))
                );
            }
        }

        // Destructor
        virtual ~Condition() = default;

        virtual void setDontCareAtRandom(double dontCareProbability) override
        {
            assert(false);
        }

        virtual std::size_t dontCareCount() const override
        {
            assert(false);

            return 0;
        }
    };

}}
