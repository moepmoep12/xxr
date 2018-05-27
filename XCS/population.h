#pragma once

#include "classifier_ptr_set.h"

template <class Symbol, typename Action>
class Population : public ClassifierPtrSet<Symbol, Action>
{
protected:
    const uint64_t m_maxPopulationClassifierCount;
    const double m_thetaDel;

    double deletionVote(const Classifier<Symbol, Action> & cl, double averageFitness, double thetaDel) const
    {
        double vote = cl.actionSetSize * cl.numerosity;

        // Consider fitness for deletion vote
        if ((cl.experience > thetaDel) && (cl.fitness / cl.numerosity < averageFitness))
        {
            vote *= averageFitness / (cl.fitness / cl.numerosity);
        }

        return vote;
    }

public:
    Population(uint64_t maxPopulationClassifierCount, double thetaDel) :
        m_maxPopulationClassifierCount(maxPopulationClassifierCount),
        m_thetaDel(thetaDel)
    {}

    Population(const ClassifierPtrSet<Symbol, Action> & obj, uint64_t maxPopulationClassifierCount, double thetaDel) :
        ClassifierPtrSet(obj),
        m_maxPopulationClassifierCount(maxPopulationClassifierCount),
        m_thetaDel(thetaDel)
    {}

    Population(const Population<Symbol, Action> & obj) :
        ClassifierPtrSet(obj),
        m_maxPopulationClassifierCount(obj.m_maxPopulationClassifierCount),
        m_thetaDel(obj.m_thetaDel)
    {}

    void insertOrIncrementNumerosity(const Classifier<Symbol, Action> & cl)
    {
        for (auto && c : m_set)
        {
            if (static_cast<ConditionActionPair<Symbol, Action>>(*c).equals(cl))
            {
                ++c->numerosity;
                return;
            }
        }
        m_set.insert(std::make_shared<Classifier<Symbol, Action>>(cl));
    }

    void deleteExtraClassifiers()
    {
        uint64_t numerositySum = 0;
        double fitnessSum = 0.0;
        for (auto && c : m_set)
        {
            numerositySum += c->numerosity;
            fitnessSum += c->fitness;
        }

        // Return if the sum of numerosity has not met its maximum limit
        if (numerositySum < m_maxPopulationClassifierCount)
        {
            return;
        }

        // The average fitness in the population
        double averageFitness = fitnessSum / numerositySum;

        // Prepare a roulette wheel by the weights
        double voteSum = 0.0;
        std::vector<double> rouletteWheel(std::size(m_set));
        std::vector<const ClassifierPtr *> rouletteWheelTarget(std::size(m_set));
        for (auto && c : m_set)
        {
            voteSum += deletionVote(*c, averageFitness, m_thetaDel);
            rouletteWheel.push_back(voteSum);
            rouletteWheelTarget.push_back(&c);
        }

        // Spin the roulette wheel
        auto rouletteIterator = std::lower_bound(std::begin(rouletteWheel), std::end(rouletteWheel), Random::nextDouble(0.0, voteSum));
        auto rouletteIdx = std::distance(std::begin(rouletteWheel), rouletteIterator);

        // Distrust the selected classifier
        if ((*rouletteWheelTarget[rouletteIdx])->numerosity > 1)
        {
            (*rouletteWheelTarget[rouletteIdx])->numerosity--;
        }
        else
        {
            m_set.erase(*rouletteWheelTarget[rouletteIdx]);
        }
    }
};