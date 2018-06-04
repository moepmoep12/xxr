#pragma once

#include <algorithm>

#include "../XCS/ga.h"

namespace XCSR
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class ClassifierPtrSet>
    class GA : public XCS::GA<T, Action, Symbol, Condition, Classifier, Population, ClassifierPtrSet>
    {
    protected:
        // APPLY MUTATION
        virtual void mutate(Classifier & cl, const std::vector<T> & situation) override const
        {
            assert(cl.condition.size() == situation.size());

            // Mutate center and spread individually
            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (XCS::Random::nextDouble() < m_mutationProbability)
                {
                    cl.condition[i].center += XCS::Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                    cl.condition[i].center = std::min(std::max(m_constants.minValue, cl.condition[i].center), m_constants.maxValue);
                }

                if (XCS::Random::nextDouble() < m_mutationProbability)
                {
                    cl.condition[i].spread += XCS::Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                }
            }

            if ((XCS::Random::nextDouble() < m_mutationProbability) && (m_actionChoices.size() >= 2))
            {
                std::unordered_set<Action> otherPossibleActions(m_actionChoices);
                otherPossibleActions.erase(cl.action);
                cl.action = XCS::Random::chooseFrom(otherPossibleActions);
            }
        }

    public:
        // Destructor
        virtual ~GA() = default;
    };

}