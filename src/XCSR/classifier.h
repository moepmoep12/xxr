#pragma once

#include "../XCS/classifier.h"

namespace XCSR
{

    template <typename T, typename Action, class Symbol, class Condition, class ConditionActionPair, class Constants>
    struct Classifier : XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>
    {
    protected:
        double m_subsumptionTolerance;

    public:
        using XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::isSubsumer;
        using XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>::isMoreGeneral;

        Classifier(const Condition & condition, Action action, uint64_t timeStamp, const Constants & constants) :
            XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>(condition, action, timeStamp, constants),
            m_subsumptionTolerance(constants.subsumptionTolerance)
        {
        }

        // DOES SUBSUME
        virtual bool subsumes(const XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants> & cl) const override
        {
            return isSubsumer() && isMoreGeneral(cl, m_subsumptionTolerance);
        }
    };

}
