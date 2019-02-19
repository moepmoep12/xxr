#pragma once

#include "../xcs/classifier.hpp"

namespace xxr { namespace xcsr_impl
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
                if (cl.condition.at(i).lower() < condition.at(i).lower() - tolerance || condition.at(i).upper() + tolerance < cl.condition.at(i).upper())
                {
                    return false;
                }

                if (cl.condition.at(i).lower() == condition.at(i).lower() - tolerance && condition.at(i).upper() + tolerance == cl.condition.at(i).upper())
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

    template <typename T, typename Action, class Symbol, class Condition, class ConditionActionPair, class Constants>
    struct Classifier : xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>
    {
    public:
        using xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::action;
        using xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::isSubsumer;
        using xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::isMoreGeneral;
        using xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::m_constants;

        Classifier(const Condition & condition, Action action, uint64_t timeStamp, Constants & constants) :
            xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>(condition, action, timeStamp, constants)
        {
        }

        // DOES SUBSUME
        virtual bool subsumes(const xcs_impl::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants> & cl) const override
        {
            return action == cl.action && isSubsumer() && isMoreGeneral(cl, m_constants.subsumptionTolerance);
        }
    };

}}
