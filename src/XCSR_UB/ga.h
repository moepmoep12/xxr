#pragma once

#include <algorithm>

#include "../XCS/ga.h"

namespace XCSR_UB
{
    
    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class Constants, class ClassifierPtrSet>
    class GA : public XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>
    {
    protected:
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_constants;
        using XCS::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_availableActions;

        // APPLY CROSSOVER
        virtual void crossover(Classifier & cl1, Classifier & cl2) const override
        {
            assert(cl1.condition.size() == cl2.condition.size());

            std::size_t x = XCS::Random::nextInt<std::size_t>(0, cl1.condition.size() * 2);
            std::size_t y = XCS::Random::nextInt<std::size_t>(0, cl1.condition.size() * 2);

            if (x > y)
            {
                std::swap(x, y);
            }

            for (std::size_t i = x + 1; i < y; ++i)
            {
                if (i % 2 == 0)
                {
                    std::swap(cl1.condition[i / 2].lower, cl2.condition[i / 2].lower);
                }
                else
                {
                    std::swap(cl1.condition[i / 2].upper, cl2.condition[i / 2].upper);
                }
            }

            // Fix lower and upper order
            for (std::size_t i = (x + 1) / 2; i < y / 2; ++i)
            {
                if (cl1.condition[i].lower > cl1.condition[i].upper)
                {
                    std::swap(cl1.condition[i].lower, cl1.condition[i].upper);
                }

                if (cl2.condition[i].lower > cl2.condition[i].upper)
                {
                    std::swap(cl2.condition[i].lower, cl2.condition[i].upper);
                }
            }
        }

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