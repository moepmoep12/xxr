#pragma once

#include <algorithm>

#include "../../xcs/ga.hpp"

namespace xxr { namespace xcsr_impl { namespace obr
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

        // APPLY CROSSOVER (uniform crossover)
        virtual void uniformCrossover(ClassifierType & cl1, ClassifierType & cl2) const override
        {
            assert(cl1.condition.size() == cl2.condition.size());

            for (std::size_t i = 0; i < cl1.condition.size(); ++i)
            {
                if (Random::nextDouble() < 0.5)
                {
                    std::swap(cl1.condition[i].l, cl2.condition[i].l);
                }
                if (Random::nextDouble() < 0.5)
                {
                    std::swap(cl1.condition[i].u, cl2.condition[i].u);
                }
            }

            // Fix lower and upper order
            for (std::size_t i = 0; i < cl1.condition.size(); ++i)
            {
                if (cl1.condition[i].l > cl1.condition[i].u)
                {
                    std::swap(cl1.condition[i].l, cl1.condition[i].u);
                }

                if (cl2.condition[i].l > cl2.condition[i].u)
                {
                    std::swap(cl2.condition[i].l, cl2.condition[i].u);
                }
            }
        }

        // APPLY CROSSOVER (one point crossover)
        virtual void onePointCrossover(ClassifierType & cl1, ClassifierType & cl2) const override
        {
            assert(cl1.condition.size() == cl2.condition.size());

            std::size_t x = Random::nextInt<std::size_t>(0, cl1.condition.size() * 2);

            for (std::size_t i = x + 1; i < cl1.condition.size() * 2; ++i)
            {
                if (i % 2 == 0)
                {
                    std::swap(cl1.condition[i / 2].l, cl2.condition[i / 2].l);
                }
                else
                {
                    std::swap(cl1.condition[i / 2].u, cl2.condition[i / 2].u);
                }
            }

            // Fix lower and upper order
            for (std::size_t i = (x + 1) / 2; i < cl1.condition.size(); ++i)
            {
                if (cl1.condition[i].l > cl1.condition[i].u)
                {
                    std::swap(cl1.condition[i].l, cl1.condition[i].u);
                }

                if (cl2.condition[i].l > cl2.condition[i].u)
                {
                    std::swap(cl2.condition[i].l, cl2.condition[i].u);
                }
            }
        }

        // APPLY CROSSOVER (two point crossover)
        virtual void twoPointCrossover(ClassifierType & cl1, ClassifierType & cl2) const override
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
                    std::swap(cl1.condition[i / 2].l, cl2.condition[i / 2].l);
                }
                else
                {
                    std::swap(cl1.condition[i / 2].u, cl2.condition[i / 2].u);
                }
            }

            // Fix lower and upper order
            for (std::size_t i = (x + 1) / 2; i < y / 2; ++i)
            {
                if (cl1.condition[i].l > cl1.condition[i].u)
                {
                    std::swap(cl1.condition[i].l, cl1.condition[i].u);
                }

                if (cl2.condition[i].l > cl2.condition[i].u)
                {
                    std::swap(cl2.condition[i].l, cl2.condition[i].u);
                }
            }
        }

        // APPLY MUTATION
        virtual void mutate(ClassifierType & cl, const std::vector<type> & situation) const override
        {
            assert(cl.condition.size() == situation.size());

            // Mutate lower or upper
            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_constants.mu)
                {
                    if (Random::nextDouble() < 0.5)
                    {
                        cl.condition[i].l += Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                        if (m_constants.doRangeRestriction)
                        {
                            cl.condition[i].l = std::min(std::max(m_constants.minValue, cl.condition[i].l), m_constants.maxValue);
                        }
                    }
                    else
                    {
                        cl.condition[i].u += Random::nextDouble(-m_constants.mutationMaxChange, m_constants.mutationMaxChange);
                        if (m_constants.doRangeRestriction)
                        {
                            cl.condition[i].u = std::min(std::max(m_constants.minValue, cl.condition[i].u), m_constants.maxValue);
                        }
                    }
                }

                if (cl.condition[i].l > cl.condition[i].u)
                {
                    std::swap(cl.condition[i].l, cl.condition[i].u);
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
