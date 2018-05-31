#pragma once

#include <unordered_map>
#include <cstdint>

#include "xcs_constants.h"
#include "classifier_ptr_set.h"
#include "population.h"

template <class Symbol, typename Action>
class MatchSet : public ClassifierPtrSet<Symbol, Action>
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;
    using ClassifierPtrSet<Symbol, Action>::m_set;
    using ClassifierPtrSet<Symbol, Action>::m_constants;
    using ClassifierPtrSet<Symbol, Action>::m_actionChoices;

public:
    using ClassifierPtrSet<Symbol, Action>::ClassifierPtrSet;

    MatchSet(Population<Symbol, Action> & population, const Situation<Symbol> & situation, uint64_t timeStamp, const XCSConstants & constants, const std::unordered_set<Action> & actionChoices) :
        ClassifierPtrSet<Symbol, Action>(constants, actionChoices)
    {
        regenerate(population, situation, timeStamp);
    }

    void regenerate(Population<Symbol, Action> & population, const Situation<Symbol> & situation, uint64_t timeStamp)
    {
        auto unselectedActions = m_actionChoices;

        m_set.clear();

        while (m_set.empty())
        {
            for (auto && cl : population)
            {
                if (cl->condition.contains(situation))
                {
                    m_set.insert(cl);
                    unselectedActions.erase(cl->action);
                }
            }

            // Generate classifiers covering the unselected actions
            if (!unselectedActions.empty())
            {
                // Generate a more general condition than the situation
                Situation<Symbol> condition(situation);
                condition.randomGeneralize(m_constants.generalizeProbability);

                auto coveringClassifier = std::make_shared<Classifier<Symbol, Action>>(condition, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
                population.insert(coveringClassifier);
                population.deleteExtraClassifiers();
                m_set.clear();
            }
        }
    }
};