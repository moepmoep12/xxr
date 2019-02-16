#pragma once

#include "../../xcs/classifier.hpp"

namespace xxr { namespace xcsr_impl { namespace csr
{

    template <typename T, typename Action, class Symbol, class Condition>
    struct ConditionActionPair : public xcs_impl::ConditionActionPair<T, Action, Symbol, Condition>
    {
        using xcs_impl::ConditionActionPair<T, Action, Symbol, Condition>::condition;
        using xcs_impl::ConditionActionPair<T, Action, Symbol, Condition>::action;

        // Constructor
        using xcs_impl::ConditionActionPair<T, Action, Symbol, Condition>::ConditionActionPair;

        // Destructor
        virtual ~ConditionActionPair() = default;

        // IS MORE GENERAL
        virtual bool isMoreGeneral(const xcs_impl::ConditionActionPair<T, Action, Symbol, Condition> & cl) const override
        {
            assert(false);
            return false;
        }

        virtual bool isMoreGeneral(const xcs_impl::ConditionActionPair<T, Action, Symbol, Condition> & cl, double tolerance) const
        {
            if (&cl == this)
            {
                // Do not subsume itself when tolerance > 0
                return false;
            }

            assert(condition.size() == cl.condition.size());

            std::size_t equalCount = 0;

            for (std::size_t i = 0; i < condition.size(); ++i)
            {
                if ((cl.condition.at(i).center - cl.condition.at(i).spread < condition.at(i).center - condition.at(i).spread - tolerance)
                    || (condition.at(i).center + condition.at(i).spread + tolerance < cl.condition.at(i).center + cl.condition.at(i).spread))
                {
                    return false;
                }

                if (condition.at(i).center == cl.condition.at(i).center && condition.at(i).spread + tolerance == cl.condition.at(i).spread)
                {
                    ++equalCount;
                }
            }

            if (equalCount == condition.size())
            {
                return false;
            }

            return true;
        }
    };

}}}
