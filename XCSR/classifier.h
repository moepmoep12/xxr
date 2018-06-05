#pragma once

#include "../XCS/classifier.h"

namespace XCSR
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
            assert(condition.size() == cl.condition.size());

            std::size_t equalCount = 0;

            for (std::size_t i = 0; i < condition.size(); ++i)
            {
                if ((cl.condition.at(i).center - cl.condition.at(i).spread < condition.at(i).center - condition.at(i).spread) && (condition.at(i).center + condition.at(i).spread < cl.condition.at(i).center + cl.condition.at(i).spread))
                {
                    return false;
                }

                if (condition.at(i).center == cl.condition.at(i).center && condition.at(i).spread == cl.condition.at(i).spread)
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