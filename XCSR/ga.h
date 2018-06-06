#pragma once

#include <algorithm>

#include "../XCS/ga.h"

namespace XCSR
{
    
    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class Constants, class ClassifierPtrSet>
    class GA : public XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>
    {
    protected:
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_constants;
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_actionChoices;

        // APPLY MUTATION
        virtual void mutate(Classifier & cl, const std::vector<T> & situation) const override
        {
            assert(cl.condition.size() == situation.size());

            // Mutate center and spread individually
            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (XCS::Random::nextDouble() < m_constants.mutationProbability)
                {
                    cl.condition[i].center += XCS::Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                    cl.condition[i].center = std::min(std::max(m_constants.minValue, cl.condition[i].center), m_constants.maxValue);
                }

                if (XCS::Random::nextDouble() < m_constants.mutationProbability)
                {
                    cl.condition[i].spread += XCS::Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                    cl.condition[i].spread = std::min(std::max(0.0, cl.condition[i].spread), m_constants.maxSpread);
                }
            }

            if ((XCS::Random::nextDouble() < m_constants.mutationProbability) && (m_actionChoices.size() >= 2))
            {
                std::unordered_set<Action> otherPossibleActions(m_actionChoices);
                otherPossibleActions.erase(cl.action);
                cl.action = XCS::Random::chooseFrom(otherPossibleActions);
            }
        }

    public:
        // Constructor
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::GA;

        // Destructor
        virtual ~GA() = default;
    };

}