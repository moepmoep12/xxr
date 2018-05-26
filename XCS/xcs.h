#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <deque>
#include <unordered_set>
#include <cstdint>
#include <cstddef>
#include <cmath>

#include "xcs_constants.h"
#include "symbol.h"
#include "classifier.h"
#include "environment.h"
#include "random.h"

template <class Environment, class Symbol, typename Action>
class XCS
{
protected:
    // [P]
    //   The population [P] consists of all classifier that exist in XCS at any time.
    std::deque<Classifier<Symbol, Action>> m_population;

    // [M]
    //   The match set [M] is formed out of the current [P].
    //   It includes all classifiers that match the current situation.
    std::deque<Classifier<Symbol, Action>> m_matchSet;

    // [A]
    //   The action set [A] is formed out of the current [M].
    //   It includes all classifiers of [M] that propose the executed action.
    std::deque<Classifier<Symbol, Action>> m_actionSet;

    // [A]_-1
    //   The previous action set [A]_-1 is the action set that was active in the last
    //   execution cycle.
    std::deque<Classifier<Symbol, Action>> m_prevActionSet;

    XCSConstants m_constants;

    uint64_t m_timeStamp;

    void generateMatchSet(const Situation<Symbol> & situation)
    {
        m_matchSet.clear();

        auto unselectedActions = m_environment.actionChoices;

        for (auto && cl : m_population)
        {
            if (cl.condition.contains(situation))
            {
                m_matchSet.push_back(cl);
                unselectedActions.erase(cl.action);
            }
        }

        // Generate classifiers covering the unselected actions
        for (Action action : unselectedActions)
        {
            auto coveringClassifier = generateCoveringClassifier(situation, action);
            m_population.push_back(coveringClassifier);
            deleteFromPopulation();
            m_matchSet.push_back(coveringClassifier);
        }
    }

    void generateActionSet(Action action)
    {
        m_actionSet.clear();

        for (auto && cl : m_matchSet)
        {
            if (cl.action == action)
            {
                m_actionSet.push_back(cl);
            }
        }
    }

    auto generateCoveringClassifier(const Situation<Symbol> & situation, Action action)
    {
        // Generate a more general condition than the situation
        auto condition = situation;
        condition.randomGeneralize(m_constants.generalizeProbability);

        // Generate a classifier
        Classifier<Symbol, Action> cl(condition, action, m_timeStamp, m_constants);
        return cl;
    }

    void deleteFromPopulation()
    {
        // Return if the sum of numerosity has not met its maximum limit
        uint64_t numerositySum = 0;
        for (auto && c : m_population)
        {
            numerositySum += c.numerosity;
        }
        if (numerositySum < m_constants.maxPopulationClassifierCount)
        {
            return;
        }

        // Prepare a roulette wheel by the weights
        double voteSum = 0.0;
        std::vector<double> rouletteWheel(std::size(m_population));
        for (auto && c : m_population)
        {
            voteSum += deletionVote(c);
            rouletteWheel.push_back(voteSum);
        }

        // Spin the roulette wheel
        auto rouletteIterator = std::lower_bound(std::begin(rouletteWheel), std::end(rouletteWheel), Random::nextDouble(0.0, voteSum));
        auto populationIterator = std::begin(m_population) + std::distance(std::begin(rouletteWheel), rouletteIterator);

        // Distrust the selected classifier
        if ((*populationIterator).numerosity > 1)
            (*populationIterator).numerosity--;
        else
            m_population.erase(populationIterator);
    }

    double deletionVote(const Classifier<Symbol, Action> & cl)
    {
        double vote = cl.actionSetSize * cl.numerosity;

        // Calculate the average fitness in the population
        double fitnessSum = 0.0;
        uint64_t numerositySum = 0;
        for (auto && c : m_population)
        {
            fitnessSum += c.fitness;
            numerositySum += c.numerosity;
        }
        double averageFitness = fitnessSum / numerositySum;

        // Consider the fitness
        if ((cl.experience > m_constants.thetaDel) && (cl.fitness / cl.numerosity < averageFitness))
        {
            vote *= averageFitness / (cl.fitness / cl.numerosity);
        }

        return vote;
    }

    void updateSet(double p)
    {
        // Calculate numerosity sum used for updating action set size estimate
        uint64_t numerositySum = 0;
        for (auto && cl : m_actionSet)
        {
            numerositySum += cl.numerosity;
        }

        for (auto && cl : m_actionSet)
        {
            ++cl.experience;
            
            // Update prediction, prediction error, and action set size estimate
            if (cl.experience < 1.0 / m_constants.learningRate)
            {
                cl.prediction += (p - cl.prediction) / cl.experience;
                cl.predictionError += (fabs(p - cl.prediction) - cl.predictionError) / cl.experience;
                cl.actionSetSize += (numerositySum - cl.actionSetSize) / cl.experience;
            }
            else
            {
                cl.prediction += m_constants.learningRate * (p - cl.prediction);
                cl.predictionError += m_constants.learningRate * (fabs(p - cl.prediction) - cl.predictionError);
                cl.actionSetSize += m_constants.learningRate * (numerositySum - cl.actionSetSize);
            }
        }

        updateFitness();

        // TODO: action set subsumption
    }

    void updateFitness()
    {
        double accuracySum = 0.0;

        // Accuracy vector
        std::vector<double> kappa(std::size(m_actionSet));

        for (auto && cl : m_actionSet)
        {
            double kappaCl;

            if (cl.predictionError < m_constants.predictionErrorThreshold)
            {
                kappaCl = 1.0;
            }
            else
            {
                kappaCl = m_constants.alpha * pow(cl.predictionError / m_constants.predictionErrorThreshold, -m_constants.nu);
            }
            kappa.push_back(kappaCl);

            accuracySum += kappaCl * cl.numerosity;
        }

        auto kappaIt = std::begin(kappa);
        for (auto && cl : m_actionSet)
        {
            cl.fitness += m_constants.learningRate * (*kappaIt * cl.numerosity / accuracySum - cl.fitness);
            ++kappaIt;
        }
    }

    void doActionSetSubsumption()
    {
        Classifier<Symbol, Action> * cl = nullptr;

        for (auto && c : m_actionSet)
        {
            if (c.isSubsumer())
            {
                if ((cl == nullptr) ||
                    ((cDontCareCount = c.condition.dontCareCount()) > (clDontCareCount = cl->condition.dontCareCount())) ||
                    ((cDontCareCount == clDontCareCount) && (Random::nextDouble() < 0.5)))
                {
                    cl = &c;
                }
            }
        }

        if (cl != nullptr)
        {
            auto it = std::begin(m_actionSet);

            while (it != m_actionSet.end())
            {
                if (cl->isMoreGeneral(*it))
                {
                    cl->numerosity += (*it).numerosity;
                    m_actionSet.erase(it);
                    // FIXME: need deletion from population
                }
                else
                {
                    ++it;
                }
            }
        }
    }

public:
    Environment environment;

    XCS(const Environment & environment, const XCSConstants & constants)
        : environment(environment),
        m_constants(constants),
        m_timeStamp(0)
    {
    }
};
