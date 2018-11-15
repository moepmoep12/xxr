#pragma once

#include "../XCS/classifier.h"

namespace XCSR_LU
{

    template <typename T, typename Action, class Symbol, class Condition>
    struct ConditionActionPair : public XCS::ConditionActionPair<T, Action, Symbol, Condition>
    {
        using XCS::ConditionActionPair<T, Action, Symbol, Condition>::condition;
        using XCS::ConditionActionPair<T, Action, Symbol, Condition>::action;

        // Constructor
        using XCS::ConditionActionPair<T, Action, Symbol, Condition>::ConditionActionPair;

        // Destructor
        virtual ~ConditionActionPair() = default;

        // IS MORE GENERAL
        virtual bool isMoreGeneral(const XCS::ConditionActionPair<T, Action, Symbol, Condition> & cl) const override
        {
            assert(false);
            return false;
        }

        virtual bool isMoreGeneral(const XCS::ConditionActionPair<T, Action, Symbol, Condition> & cl, double tolerance) const
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
                if (cl.condition.at(i).lower < condition.at(i).lower - tolerance || condition.at(i).upper + tolerance < cl.condition.at(i).upper)
                {
                    return false;
                }

                if (cl.condition.at(i).lower == condition.at(i).lower - tolerance && condition.at(i).upper + tolerance == cl.condition.at(i).upper)
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

}