#pragma once

#include <algorithm>

#include "../../xcs/ga.hpp"

namespace xxr { namespace xcsr_impl { namespace obr
{
    
    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class Constants, class ClassifierPtrSet>
    class GA : public xcs_impl::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>
    {
    protected:
        using xcs_impl::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_constants;
        using xcs_impl::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::m_availableActions;

        // APPLY CROSSOVER
        virtual void crossover(Classifier & cl1, Classifier & cl2) const override
        {
            assert(cl1.condition.size() == cl2.condition.size());

            std::size_t x = Random::nextInt<std::size_t>(0, cl1.condition.size() * 2);
            std::size_t y = Random::nextInt<std::size_t>(0, cl1.condition.size() * 2);

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

            // Mutate lower or upper
            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_constants.mutationProbability)
                {
                    if (Random::nextDouble() < 0.5)
                    {
                        cl.condition[i].lower += Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                        if (m_constants.doRangeRestriction)
                        {
                            cl.condition[i].lower = std::min(std::max(m_constants.minValue, cl.condition[i].lower), m_constants.maxValue);
                        }
                    }
                    else
                    {
                        cl.condition[i].upper += Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                        if (m_constants.doRangeRestriction)
                        {
                            cl.condition[i].upper = std::min(std::max(m_constants.minValue, cl.condition[i].upper), m_constants.maxValue);
                        }
                    }
                }

                if (cl.condition[i].lower > cl.condition[i].upper)
                {
                    std::swap(cl.condition[i].lower, cl.condition[i].upper);
                }
            }

            if (m_constants.doActionMutation && (Random::nextDouble() < m_constants.mutationProbability) && (m_availableActions.size() >= 2))
            {
                std::unordered_set<Action> otherPossibleActions(m_availableActions);
                otherPossibleActions.erase(cl.action);
                cl.action = Random::chooseFrom(otherPossibleActions);
            }
        }

    public:
        // Constructor
        using xcs_impl::GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>::GA;

        // Destructor
        virtual ~GA() = default;
    };

}}}
