#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <cstddef>

namespace xxr { namespace xcs_impl
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class Constants, class ClassifierPtrSet>
    class GA
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;

        Constants & m_constants;
        const std::unordered_set<Action> & m_availableActions;

        // SELECT OFFSPRING
        virtual ClassifierPtr selectOffspring(const ClassifierPtrSet & actionSet) const
        {
            std::vector<const ClassifierPtr *> targets;
            for (auto && cl : actionSet)
            {
                targets.push_back(&cl);
            }

            std::size_t selectedIdx;
            if (m_constants.tournamentSize > 0.0 && m_constants.tournamentSize <= 1.0)
            {
                // Tournament selection
                std::vector<std::pair<double, std::size_t>> fitnesses;
                fitnesses.reserve(actionSet.size());
                for (auto && cl : actionSet)
                {
                    fitnesses.emplace_back(cl->fitness, cl->numerosity);
                }
                selectedIdx = Random::tournamentSelection(fitnesses, m_constants.tournamentSize);
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

        // APPLY CROSSOVER
        virtual void crossover(Classifier & cl1, Classifier & cl2) const
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

        // APPLY MUTATION
        virtual void mutate(Classifier & cl, const std::vector<T> & situation) const
        {
            assert(cl.condition.size() == situation.size());

            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_constants.mutationProbability)
                {
                    if (cl.condition[i].isDontCare())
                    {
                        cl.condition[i] = Symbol(situation.at(i));
                    }
                    else
                    {
                        cl.condition[i].generalize();
                    }
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
        GA(Constants & constants, const std::unordered_set<Action> & availableActions) :
            m_constants(constants),
            m_availableActions(availableActions)
        {
        }

        // Destructor
        virtual ~GA() = default;

        // RUN GA (refer to ActionSet::runGA() for the former part)
        virtual void run(ClassifierPtrSet & actionSet, const std::vector<T> & situation, Population & population) const
        {
            auto parent1 = selectOffspring(actionSet);
            auto parent2 = selectOffspring(actionSet);

            assert(parent1->condition.size() == parent2->condition.size());

            Classifier child1(*parent1);
            Classifier child2(*parent2);

            child1.fitness /= parent1->numerosity;
            child2.fitness /= parent2->numerosity;
            child1.numerosity = child2.numerosity = 1;
            child1.experience = child2.experience = 0;

            if (Random::nextDouble() < m_constants.crossoverProbability)
            {
                crossover(child1, child2);

                child1.prediction =
                    child2.prediction = (parent1->prediction + parent2->prediction) / 2;

                child1.predictionError =
                    child2.predictionError = (parent1->predictionError + parent2->predictionError) / 2;

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
                        population.insertOrIncrementNumerosity(std::make_shared<Classifier>(*child));
                    }
                }
                else
                {
                    population.insertOrIncrementNumerosity(std::make_shared<Classifier>(*child));
                }

                population.deleteExtraClassifiers();
            }
        }
    };

}}
