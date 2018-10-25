#pragma once

#include "../XCS/action_set.h"

namespace XCSR
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet, class Population, class MatchSet, class GA>
    class ActionSet : public XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::m_set;
        using XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::m_constants;
        using XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::m_availableActions;

        // DO ACTION SET SUBSUMPTION
        virtual void doSubsumption(Population & population) override
        {
            ClassifierPtr cl;

            for (auto && c : m_set)
            {
                if (c->isSubsumer())
                {
                    if (cl.get() == nullptr || c->isMoreGeneral(*cl, m_constants.subsumptionTolerance))
                    {
                        cl = c;
                    }
                }
            }

            if (cl.get() != nullptr)
            {
                std::vector<const ClassifierPtr *> removedClassifiers;
                for (auto && c : m_set)
                {
                    if (cl->isMoreGeneral(*c, m_constants.subsumptionTolerance))
                    {
                        cl->numerosity += c->numerosity;
                        removedClassifiers.push_back(&c);
                    }
                }

                for (auto && removedClassifier : removedClassifiers)
                {
                    population.erase(*removedClassifier);
                    m_set.erase(*removedClassifier);
                }
            }
        }

    public:
        // Constructor
        using XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::ActionSet;

        // Destructor
        virtual ~ActionSet() = default;
    };

}