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
#include "ga.h"
#include "environment.h"
#include "random.h"

template <class Environment, class Symbol, typename Action>
class XCS
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;
    using ClassifierPtrSet = std::unordered_set<ClassifierPtr>;

    // [P]
    //   The population [P] consists of all classifier that exist in XCS at any time.
    ClassifierPtrSet m_population;

    // [M]
    //   The match set [M] is formed out of the current [P].
    //   It includes all classifiers that match the current situation.
    ClassifierPtrSet m_matchSet;

    // [A]
    //   The action set [A] is formed out of the current [M].
    //   It includes all classifiers of [M] that propose the executed action.
    ClassifierPtrSet m_actionSet;

    // [A]_-1
    //   The previous action set [A]_-1 is the action set that was active in the last
    //   execution cycle.
    ClassifierPtrSet m_prevActionSet;

    XCSConstants m_constants;

    GA<Symbol, Action> m_ga;

    uint64_t m_timeStamp;

    void generateMatchSet(const Situation<Symbol> & situation)
    {
        m_matchSet.clear();

        auto unselectedActions = environment.actionChoices;

        for (auto && cl : m_population)
        {
            if (cl->condition.contains(situation))
            {
                m_matchSet.insert(cl);
                unselectedActions.erase(cl->action);
            }
        }

        // Generate classifiers covering the unselected actions
        for (Action action : unselectedActions)
        {
            auto coveringClassifier = generateCoveringClassifier(situation, action);
            m_population.insert(coveringClassifier);
            deleteFromPopulation();
            m_matchSet.insert(coveringClassifier);
        }
    }

    void generateActionSet(Action action)
    {
        m_actionSet.clear();

        for (auto && cl : m_matchSet)
        {
            if (cl->action == action)
            {
                m_actionSet.insert(cl);
            }
        }
    }

    auto generateCoveringClassifier(const Situation<Symbol> & situation, Action action)
    {
        // Generate a more general condition than the situation
        auto condition = situation;
        condition.randomGeneralize(m_constants.generalizeProbability);

        // Generate a classifier
        return std::make_shared<Classifier<Symbol, Action>>(condition, action, m_timeStamp, m_constants);;
    }

    void insertInPopulation(const Classifier<Symbol, Action> & cl)
    {
        for (auto && c : m_population)
        {
            if (static_cast<ConditionActionPair<Symbol, Action>>(*c).equals(cl))
            {
                ++c->numerosity;
                return;
            }
        }
        m_population.insert(std::make_shared<Classifier<Symbol, Action>>(cl));
    }

    void deleteFromPopulation()
    {
        // Return if the sum of numerosity has not met its maximum limit
        uint64_t numerositySum = 0;
        for (auto && c : m_population)
        {
            numerositySum += c->numerosity;
        }
        if (numerositySum < m_constants.maxPopulationClassifierCount)
        {
            return;
        }

        // Prepare a roulette wheel by the weights
        double voteSum = 0.0;
        std::vector<double> rouletteWheel(std::size(m_population));
        std::vector<const ClassifierPtr *> rouletteWheelTarget(std::size(m_population));
        for (auto && c : m_population)
        {
            voteSum += deletionVote(*c);
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
            m_population.erase(*rouletteWheelTarget[rouletteIdx]);
        }
    }

    double deletionVote(const Classifier<Symbol, Action> & cl)
    {
        double vote = cl.actionSetSize * cl.numerosity;

        // Calculate the average fitness in the population
        double fitnessSum = 0.0;
        uint64_t numerositySum = 0;
        for (auto && c : m_population)
        {
            fitnessSum += c->fitness;
            numerositySum += c->numerosity;
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
            numerositySum += cl->numerosity;
        }

        for (auto && cl : m_actionSet)
        {
            ++cl->experience;
            
            // Update prediction, prediction error, and action set size estimate
            if (cl->experience < 1.0 / m_constants.learningRate)
            {
                cl->prediction += (p - cl->prediction) / cl->experience;
                cl->predictionError += (fabs(p - cl->prediction) - cl->predictionError) / cl->experience;
                cl->actionSetSize += (numerositySum - cl->actionSetSize) / cl->experience;
            }
            else
            {
                cl->prediction += m_constants.learningRate * (p - cl->prediction);
                cl->predictionError += m_constants.learningRate * (fabs(p - cl->prediction) - cl->predictionError);
                cl->actionSetSize += m_constants.learningRate * (numerositySum - cl->actionSetSize);
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

            if (cl->predictionError < m_constants.predictionErrorThreshold)
            {
                kappaCl = 1.0;
            }
            else
            {
                kappaCl = m_constants.alpha * pow(cl->predictionError / m_constants.predictionErrorThreshold, -m_constants.nu);
            }
            kappa.push_back(kappaCl);

            accuracySum += kappaCl * cl->numerosity;
        }

        auto kappaItr = std::begin(kappa);
        for (auto && cl : m_actionSet)
        {
            cl->fitness += m_constants.learningRate * (*kappaItr * cl->numerosity / accuracySum - cl->fitness);
            ++kappaItr;
        }
    }

    void doActionSetSubsumption()
    {
        ClassifierPtr cl;

        for (auto && c : m_actionSet)
        {
            if (c->isSubsumer())
            {
                size_t cDontCareCount;
                size_t clDontCareCount;
                if ((cl == nullptr) ||
                    ((cDontCareCount = c->condition.dontCareCount()) > (clDontCareCount = cl->condition.dontCareCount())) ||
                    ((cDontCareCount == clDontCareCount) && (Random::nextDouble() < 0.5)))
                {
                    cl = c;
                }
            }
        }

        if (cl.get() != nullptr)
        {
            std::vector<const ClassifierPtr *> removedClassifiers;
            for (auto && c : m_actionSet)
            {
                if (cl->isMoreGeneral(*c))
                {
                    cl->numerosity += c->numerosity;
                    removedClassifiers.push_back(&c);
                }
            }

            for (auto && removedClassifier : removedClassifiers)
            {
                m_actionSet.erase(*removedClassifier);
                m_population.erase(*removedClassifier);
            }
        }
    }

    void runGA(const Situation<Symbol> & situation)
    {
        uint64_t timeStampNumerositySum = 0;
        uint64_t numerositySum = 0;

        for (auto && cl : m_actionSet)
        {
            timeStampNumerositySum += cl->timeStamp * cl->numerosity;
            numerositySum += cl->numerosity;
        }

        if (m_timeStamp - timeStampNumerositySum / numerositySum > m_constants.thetaGA)
        {
            for (auto && cl : actionSet)
            {
                cl->timeStamp = m_timeStamp;
            }

            GA.run(m_actionSet, situation, m_population);
        }
    }

public:
    Environment environment;

    XCS(const Environment & environment, const XCSConstants & constants)
        : environment(environment),
        m_ga(constants.crossoverProbability, constants.mutationProbability, environment.actionChoices),
        m_constants(constants),
        m_timeStamp(0)
    {
    }
};
