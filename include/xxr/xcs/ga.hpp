#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <cstddef>

namespace xxr { namespace xcs_impl
{

    template <class Population>
    class GA
    {
    public:
        using type = typename Population::type;
        using SymbolType = typename Population::SymbolType;
        using ConditionType = typename Population::ConditionType;
        using ActionType = typename Population::ActionType;
        using ConditionActionPairType = typename Population::ConditionActionPairType;
        using ConstantsType = typename Population::ConstantsType;
        using ClassifierType = typename Population::ClassifierType;
        using StoredClassifierType = typename Population::StoredClassifierType;
        using ClassifierPtr = typename Population::ClassifierPtr;
        using ClassifierPtrSetType = typename Population::ClassifierPtrSetType;
        using PopulationType = Population;

    protected:
        ConstantsType & m_constants;
        const std::unordered_set<ActionType> & m_availableActions;

        // SELECT OFFSPRING
        virtual ClassifierPtr selectOffspring(const ClassifierPtrSetType & actionSet) const
        {
            std::vector<const ClassifierPtr *> targets;
            for (auto && cl : actionSet)
            {
                targets.push_back(&cl);
            }

            std::size_t selectedIdx;
            if (m_constants.tau > 0.0 && m_constants.tau <= 1.0)
            {
                // Tournament selection
                std::vector<std::pair<double, std::size_t>> fitnesses;
                fitnesses.reserve(actionSet.size());
                for (auto && cl : actionSet)
                {
                    fitnesses.emplace_back(cl->fitness, cl->numerosity);
                }
                selectedIdx = Random::tournamentSelection(fitnesses, m_constants.tau);
            }
            else
            {
                // Roulette-wheel selection
                std::vector<double> fitnesses;
                fitnesses.reserve(actionSet.size());
                for (auto && cl : actionSet)
                {
                    fitnesses.push_back(cl->fitness);
                }
                selectedIdx = Random::rouletteWheelSelection(fitnesses);
            }
            return *targets[selectedIdx];
        }

        // APPLY CROSSOVER (uniform crossover)
        virtual void uniformCrossover(ClassifierType & cl1, ClassifierType & cl2) const
        {
            assert(cl1.condition.size() == cl2.condition.size());

            for (std::size_t i = 0; i < cl1.condition.size(); ++i)
            {
                if (Random::nextDouble() < 0.5)
                {
                    std::swap(cl1.condition[i], cl2.condition[i]);
                }
            }
        }

        // APPLY CROSSOVER (one point crossover)
        virtual void onePointCrossover(ClassifierType & cl1, ClassifierType & cl2) const
        {
            assert(cl1.condition.size() == cl2.condition.size());

            std::size_t x = Random::nextInt<std::size_t>(0, cl1.condition.size());

            for (std::size_t i = x + 1; i < cl1.condition.size(); ++i)
            {
                std::swap(cl1.condition[i], cl2.condition[i]);
            }
        }

        // APPLY CROSSOVER (two point crossover)
        virtual void twoPointCrossover(ClassifierType & cl1, ClassifierType & cl2) const
        {
            assert(cl1.condition.size() == cl2.condition.size());

            std::size_t x = Random::nextInt<std::size_t>(0, cl1.condition.size());
            std::size_t y = Random::nextInt<std::size_t>(0, cl1.condition.size());

            if (x > y)
            {
                std::swap(x, y);
            }

            for (std::size_t i = x + 1; i < y; ++i)
            {
                std::swap(cl1.condition[i], cl2.condition[i]);
            }
        }

        // APPLY CROSSOVER
        virtual void crossover(ClassifierType & cl1, ClassifierType & cl2) const
        {
            switch (m_constants.crossoverMethod)
            {
            case Constants::CrossoverMethod::UNIFORM_CROSSOVER:
                uniformCrossover(cl1, cl2);
                break;

            case Constants::CrossoverMethod::ONE_POINT_CROSSOVER:
                onePointCrossover(cl1, cl2);
                break;

            case Constants::CrossoverMethod::TWO_POINT_CROSSOVER:
                twoPointCrossover(cl1, cl2);
                break;
            }
        }

        // APPLY MUTATION
        virtual void mutate(ClassifierType & cl, const std::vector<type> & situation) const
        {
            assert(cl.condition.size() == situation.size());

            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_constants.mu)
                {
                    if (cl.condition[i].isDontCare())
                    {
                        cl.condition[i] = SymbolType(situation.at(i));
                    }
                    else
                    {
                        cl.condition[i].setDontCare();
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
        GA(ConstantsType & constants, const std::unordered_set<ActionType> & availableActions) :
            m_constants(constants),
            m_availableActions(availableActions)
        {
        }

        // Destructor
        virtual ~GA() = default;

        // RUN GA (refer to ActionSet::runGA() for the former part)
        virtual void run(ClassifierPtrSetType & actionSet, const std::vector<type> & situation, PopulationType & population) const
        {
            auto parent1 = selectOffspring(actionSet);
            auto parent2 = selectOffspring(actionSet);

            assert(parent1->condition.size() == parent2->condition.size());

            ClassifierType child1(*parent1);
            ClassifierType child2(*parent2);

            child1.fitness /= parent1->numerosity;
            child2.fitness /= parent2->numerosity;
            child1.numerosity = child2.numerosity = 1;
            child1.experience = child2.experience = 0;

            if (Random::nextDouble() < m_constants.chi)
            {
                crossover(child1, child2);

                child1.prediction =
                    child2.prediction = (parent1->prediction + parent2->prediction) / 2;

                child1.epsilon =
                    child2.epsilon = (parent1->epsilon + parent2->epsilon) / 2;

                child1.fitness =
                    child2.fitness = (parent1->fitness + parent2->fitness) / 2 * 0.1;
            }

            for (auto && child : { &child1, &child2 })
            {
                mutate(*child, situation);

                if (m_constants.doGASubsumption)
                {
                    if (parent1->subsumes(*child))
                    {
                        ++parent1->numerosity;
                    }
                    else if (parent2->subsumes(*child))
                    {
                        ++parent2->numerosity;
                    }
                    else
                    {
                        population.insertOrIncrementNumerosity(std::make_shared<StoredClassifierType>(*child, m_constants));
                    }
                }
                else
                {
                    population.insertOrIncrementNumerosity(std::make_shared<StoredClassifierType>(*child, m_constants));
                }

                population.deleteExtraClassifiers();
            }
        }
    };

}}
