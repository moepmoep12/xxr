#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <cstddef>

#include "classifier.h"
#include "classifier_ptr_set.h"
#include "population.h"
#include "random.h"

namespace XCS
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population, class Constants, class ClassifierPtrSet>
    class GA
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;

        const Constants m_constants;
        const std::unordered_set<Action> & m_actionChoices;

        // SELECT OFFSPRING
        virtual ClassifierPtr selectOffspring(const ClassifierPtrSet & actionSet) const
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
        virtual void crossover(Classifier & cl1, Classifier & cl2) const
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
        virtual void mutate(Classifier & cl, const std::vector<T> & situation) const
        {
            assert(cl.condition.size() == situation.size());

            for (std::size_t i = 0; i < cl.condition.size(); ++i)
            {
                if (Random::nextDouble() < m_constants.mutationProbability)
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

            if ((Random::nextDouble() < m_constants.mutationProbability) && (m_actionChoices.size() >= 2))
            {
                std::unordered_set<Action> otherPossibleActions(m_actionChoices);
                otherPossibleActions.erase(cl.action);
                cl.action = Random::chooseFrom(otherPossibleActions);
            }
        }

    public:
        // Constructor
        GA(const Constants & constants, const std::unordered_set<Action> & actionChoices) :
            m_constants(constants),
            m_actionChoices(actionChoices)
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
                    child2.fitness = (parent1->fitness + parent2->fitness) / 2;
            }

            child1.fitness *= 0.1;
            child2.fitness *= 0.1;

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