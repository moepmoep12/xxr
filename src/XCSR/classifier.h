#pragma once

#include "../XCS/classifier.h"

namespace XCSR
{

    template <typename T, typename Action, class Symbol, class Condition, class ConditionActionPair, class Constants>
    struct Classifier : XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>
    {
    public:
        using XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::isSubsumer;
        using XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::isMoreGeneral;
        using XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::m_constants;

        Classifier(const Condition & condition, Action action, uint64_t timeStamp, Constants & constants) :
            XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>(condition, action, timeStamp, constants)
        {
        }

        // DOES SUBSUME
        virtual bool subsumes(const XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants> & cl) const override
        {
            return isSubsumer() && isMoreGeneral(cl, m_constants.subsumptionTolerance);
        }
    };

}
