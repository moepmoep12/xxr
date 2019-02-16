#pragma once

#include "../xcs/classifier.hpp"

namespace xxr { namespace xcsr_impl
{

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
