#pragma once

#include <algorithm>

#include "../../xcs/ga.hpp"

namespace xxr { namespace xcsr_impl { namespace csr
{

    template <class Population>
    class GA : public xcs_impl::GA<Population>
    {
    public:
        using typename xcs_impl::GA<Population>::type;
        using typename xcs_impl::GA<Population>::SymbolType;
        using typename xcs_impl::GA<Population>::ConditionType;
        using typename xcs_impl::GA<Population>::ActionType;
        using typename xcs_impl::GA<Population>::ConditionActionPairType;
        using typename xcs_impl::GA<Population>::ConstantsType;
        using typename xcs_impl::GA<Population>::ClassifierType;
        using typename xcs_impl::GA<Population>::StoredClassifierType;
        using typename xcs_impl::GA<Population>::ClassifierPtr;
        using typename xcs_impl::GA<Population>::ClassifierPtrSetType;
        using typename xcs_impl::GA<Population>::PopulationType;

    protected:
        using xcs_impl::GA<Population>::m_constants;
        using xcs_impl::GA<Population>::m_availableActions;

        // APPLY CROSSOVER
        virtual void crossover(ClassifierType & cl1, ClassifierType & cl2) const override
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
                    std::swap(cl1.condition[i / 2].center, cl2.condition[i / 2].center);
                }
                else
                {
                    std::swap(cl1.condition[i / 2].spread, cl2.condition[i / 2].spread);
                }
            }
        }

        // APPLY MUTATION
        virtual void mutate(ClassifierType & cl, const std::vector<type> & situation) const override
        {
            assert(cl.condition.size() == situation.size());

            // Mutate center or spread
            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_constants.mu)
                {
                    if (Random::nextDouble() < 0.5)
                    {
                        cl.condition[i].center += Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                        cl.condition[i].center = std::min(std::max(m_constants.minValue, cl.condition[i].center), m_constants.maxValue);
                    }
                    else
                    {
                        cl.condition[i].spread += Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                        cl.condition[i].spread = std::max(0.0, cl.condition[i].spread);
                    }
                }
            }

            if (m_constants.doActionMutation && (Random::nextDouble() < m_constants.mu) && (m_availableActions.size() >= 2))
            {
                std::unordered_set<ActionType> otherPossibleActions(m_availableActions);
                otherPossibleActions.erase(cl.action);
                cl.action = Random::chooseFrom(otherPossibleActions);
            }
        }

    public:
        // Constructor
        using xcs_impl::GA<Population>::GA;

        // Destructor
        virtual ~GA() = default;
    };

}}}
