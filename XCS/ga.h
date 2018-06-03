#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <cstddef>

#include "classifier.h"
#include "classifier_ptr_set.h"
#include "population.h"
#include "random.h"

namespace xcs
{

    template <typename T, typename Action, class Symbol = Symbol<T>>
    class GA
    {
    public:
        using ClassifierPtr = std::shared_ptr<Classifier<T, Action>>;

        const double m_crossoverProbability;
        const double m_mutationProbability;
        const bool m_doGASubsumption;

        const std::unordered_set<Action> & m_actionChoices;

        // SELECT OFFSPRING
        virtual ClassifierPtr selectOffspring(const ClassifierPtrSet<T, Action> & actionSet) const
        {
            double choicePoint;
            {
                double fitnessSum = 0.0;
                for (auto && cl : actionSet)
                {
                    fitnessSum += cl->fitness;
                }

                assert(fitnessSum > 0.0);

                choicePoint = Random::nextDouble(0.0, fitnessSum);
            }

            {
                double fitnessSum = 0.0;
                for (auto && cl : actionSet)
                {
                    fitnessSum += cl->fitness;
                    if (fitnessSum > choicePoint)
                    {
                        return cl;
                    }
                }
            }

            assert(false);

            return *std::begin(actionSet);
        }

        // APPLY CROSSOVER
        virtual void crossover(Classifier<T, Action> & cl1, Classifier<T, Action> & cl2) const
        {
            assert(cl1.condition.size() == cl2.condition.size());

            std::size_t x = static_cast<std::size_t>(Random::nextDouble() * (cl1.condition.size() + 1));
            std::size_t y = static_cast<std::size_t>(Random::nextDouble() * (cl1.condition.size() + 1));

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
        virtual void mutate(Classifier<T, Action> & cl, const std::vector<T> & situation) const
        {
            assert(cl.condition.size() == situation.size());

            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_mutationProbability)
                {
                    if (cl.condition[i].isDontCare())
                    {
                        cl.condition[i] = situation.at(i);
                    }
                    else
                    {
                        cl.condition[i].generalize();
                    }
                }
            }

            if ((Random::nextDouble() < m_mutationProbability) && (m_actionChoices.size() >= 2))
            {
                std::unordered_set<Action> otherPossibleActions(m_actionChoices);
                otherPossibleActions.erase(cl.action);
                cl.action = Random::chooseFrom(otherPossibleActions);
            }
        }

    public:
        GA(double crossoverProbability, double mutationProbability, bool doGASubsumption, const std::unordered_set<Action> & actionChoices) :
            m_crossoverProbability(crossoverProbability),
            m_mutationProbability(mutationProbability),
            m_doGASubsumption(doGASubsumption),
            m_actionChoices(actionChoices)
        {
        }

        // RUN GA (refer to ActionSet::runGA() for the former part)
        virtual void run(ClassifierPtrSet<T, Action> & actionSet, const std::vector<T> & situation, Population<T, Action> & population) const
        {
            auto parent1 = selectOffspring(actionSet);
            auto parent2 = selectOffspring(actionSet);

            assert(parent1->condition.size() == parent2->condition.size());

            Classifier<T, Action> child1(*parent1);
            Classifier<T, Action> child2(*parent2);

            child1.numerosity = child2.numerosity = 1;
            child1.experience = child2.experience = 0;

            if (Random::nextDouble() < m_crossoverProbability)
            {
                crossover(child1, child2);

                child1.prediction =
                    child2.prediction = (parent1->prediction + parent2->prediction) / 2;

                child1.predictionError =
                    child2.predictionError = (parent1->predictionError + parent2->predictionError) / 2;

                child1.fitness =
                    child2.fitness = (parent1->fitness + parent2->fitness) / 2;
            }

            child1.fitness *= 0.1;
            child2.fitness *= 0.1;

            for (auto && child : { &child1, &child2 })
            {
                mutate(*child, situation);

                if (m_doGASubsumption)
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
                        population.insertOrIncrementNumerosity(*child);
                    }
                }
                else
                {
                    population.insertOrIncrementNumerosity(*child);
                }

                population.deleteExtraClassifiers();
            }
        }
    };

}