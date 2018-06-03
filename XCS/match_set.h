#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>

#include "xcs_constants.h"
#include "classifier_ptr_set.h"
#include "population.h"

template <typename T, typename Action, class Symbol = Symbol<T>>
class MatchSet : public ClassifierPtrSet<T, Action>
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier<T, Action>>;
    using ClassifierPtrSet<T, Action>::m_set;
    using ClassifierPtrSet<T, Action>::m_constants;
    using ClassifierPtrSet<T, Action>::m_actionChoices;

    // GENERATE COVERING CLASSIFIER
    virtual ClassifierPtr generateCoveringClassifier(const std::vector<T> & situation, const std::unordered_set<Action> & unselectedActions, uint64_t timeStamp) const
    {
        auto cl = std::make_shared<Classifier<T, Action>>(situation, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
        cl->condition.randomGeneralize(m_constants.generalizeProbability);

        return cl;
    }

public:
    using ClassifierPtrSet<T, Action>::ClassifierPtrSet;

    MatchSet(Population<T, Action> & population, const std::vector<T> & situation, uint64_t timeStamp, const XCSConstants & constants, const std::unordered_set<Action> & actionChoices) :
        ClassifierPtrSet<T, Action>(constants, actionChoices)
    {
        regenerate(population, situation, timeStamp);
    }

    // GENERATE MATCH SET
    virtual void regenerate(Population<T, Action> & population, const std::vector<T> & situation, uint64_t timeStamp)
    {
        // Set theta_mna (the minimal number of actions) to the number of action choices if theta_mna is 0
        auto thetaMna = (m_constants.thetaMna == 0) ? m_actionChoices.size() : m_constants.thetaMna;

        auto unselectedActions = m_actionChoices;

        m_set.clear();

        while (m_set.empty())
        {
            for (auto && cl : population)
            {
                if (cl->condition.matches(situation))
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