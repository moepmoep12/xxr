#pragma once

#include <memory>
#include <unordered_set>
#include <cassert>
#include <cstddef>

#include "classifier.h"
#include "random.h"

template <class Symbol, typename Action>
class GA
{
public:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;
    using ClassifierPtrSet = std::unordered_set<ClassifierPtr>;

    const double m_crossoverProbability;
    const double m_mutationProbability;
    const bool m_doGASubsumption;

    const std::unordered_set<Action> & m_actionChoices;

    ClassifierPtr selectOffspring(const ClassifierPtrSet & actionSet) const
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

        return *std::begin(actionSet);
    }

    void crossover(Classifier<Symbol, Action> & cl1, Classifier<Symbol, Action> & cl2) const
    {
        assert(std::size(cl1.condition) == std::size(cl2.condition));

        size_t x = static_cast<size_t>(Random::nextDouble() * (std::size(cl1.condition) + 1));
        size_t y = static_cast<size_t>(Random::nextDouble() * (std::size(cl1.condition) + 1));

        if (x > y)
        {
            std::swap(x, y);
        }

        for (size_t i = x; i < y; ++i)
        {
            std::swap(cl1.condition[i], cl2.condition[i]);
        }
    }

    void mutate(Classifier<Symbol, Action> & cl, const Situation<Symbol> & situation) const
    {
        assert(std::size(cl.condition) == std::size(situation));

        for (int i = 0; i < std::size(cl.condition); ++i)
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

        if ((Random::nextDouble() < m_mutationProbability) && (std::size(m_actionChoices) >= 2))
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

    void run(ClassifierPtrSet & actionSet, const Situation<Symbol> & situation, ClassifierPtrSet & population) const
    {
        auto parent1 = selectOffspring(actionSet);
        auto parent2 = selectOffspring(actionSet);

        assert(parent1->condition.size() == parent2->condition.size());

        auto child1 = *parent1;
        auto child2 = *parent2;

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
                    // FIXME: insertInPopulation(*child);
                }
            }
            else
            {
                // FIXME: insertInPopulation(*child);
            }

            // FIXME: deleteFromPopulation();
        }
    }
};