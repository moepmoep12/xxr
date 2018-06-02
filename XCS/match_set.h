#pragma once

#include <memory>
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

    // GENERATE COVERING CLASSIFIER
    auto generateCoveringClassifier(const Situation<Symbol> & situation, const std::unordered_set<Action> & unselectedActions, uint64_t timeStamp) const
    {
        auto cl = std::make_shared<Classifier<Symbol, Action>>(situation, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
        cl->condition.randomGeneralize(m_constants.generalizeProbability);

        return cl;
    }

public:
    using ClassifierPtrSet<Symbol, Action>::ClassifierPtrSet;

    MatchSet(Population<Symbol, Action> & population, const Situation<Symbol> & situation, uint64_t timeStamp, const XCSConstants & constants, const std::unordered_set<Action> & actionChoices) :
        ClassifierPtrSet<Symbol, Action>(constants, actionChoices)
    {
        regenerate(population, situation, timeStamp);
    }

    // GENERATE MATCH SET
    void regenerate(Population<Symbol, Action> & population, const Situation<Symbol> & situation, uint64_t timeStamp)
    {
        // Set theta_mna (the minimal number of actions) to the number of action choices if theta_mna is 0
        auto thetaMna = (m_constants.thetaMna == 0) ? m_actionChoices.size() : m_constants.thetaMna;

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
            if ((m_actionChoices.size() - unselectedActions.size() < thetaMna)) 
            {
                population.insert(generateCoveringClassifier(situation, unselectedActions, timeStamp));
                population.deleteExtraClassifiers();
                m_set.clear();
            }
        }
    }
};