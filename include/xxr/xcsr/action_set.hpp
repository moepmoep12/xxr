#pragma once

#include "../xcs/action_set.hpp"

namespace xxr { namespace xcsr_impl
{

    template <typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet, class Population, class MatchSet, class GA>
    class ActionSet : public xcs_impl::ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using xcs_impl::ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::m_set;
        using xcs_impl::ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::m_constants;
        using xcs_impl::ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::m_availableActions;

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
        using xcs_impl::ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>::ActionSet;

        // Destructor
        virtual ~ActionSet() = default;
    };

}}
