#pragma once

#include <algorithm>

#include "../XCS/ga.h"

namespace XCSR_LU
{
    
    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class Constants, class ClassifierPtrSet>
    class GA : public XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>
    {
    protected:
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_constants;
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_availableActions;

        // APPLY MUTATION
        virtual void mutate(Classifier & cl, const std::vector<T> & situation) const override
        {
            assert(cl.condition.size() == situation.size());

            // Mutate lower and upper individually
            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (XCS::Random::nextDouble() < m_constants.mutationProbability)
                {
                    cl.condition[i].lower += XCS::Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                    cl.condition[i].lower = std::min(std::max(m_constants.minValue, cl.condition[i].lower), m_constants.maxValue);
                }

                if (XCS::Random::nextDouble() < m_constants.mutationProbability)
                {
                    cl.condition[i].upper += XCS::Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                    cl.condition[i].upper = std::min(std::max(m_constants.minValue, cl.condition[i].upper), m_constants.maxValue);
                }

                if (cl.condition[i].lower > cl.condition[i].upper)
                {
                    std::swap(cl.condition[i].lower, cl.condition[i].upper);
                }
            }

            if ((XCS::Random::nextDouble() < m_constants.mutationProbability) && (m_availableActions.size() >= 2))
            {
                std::unordered_set<Action> otherPossibleActions(m_availableActions);
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