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

    auto generateCoveringClassifier(const Situation<Symbol> & situation, Action action) const
    {

        // Generate a classifier
        return std::make_shared<Classifier<Symbol, Action>>(condition, action, m_timeStamp, m_constants);
    }

public:
    MatchSet() = default;

    MatchSet(Population<Symbol, Action> & population, const Situation<Symbol> & situation, const std::unordered_set<Action> actionChoices, uint64_t timeStamp, const XCSConstants & constants)
    {
        auto unselectedActions = actionChoices;

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
                condition.randomGeneralize(constants.generalizeProbability);

                auto coveringClassifier = std::make_shared<Classifier<Symbol, Action>>(condition, Random::chooseFrom(unselectedActions), timeStamp, constants);
                population.insert(coveringClassifier);
                population.deleteExtraClassifiers();
                m_set.clear();
            }
        }
    }
};