#pragma once

#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cmath>

#include "xcs_constants.h"
#include "symbol.h"
#include "classifier.h"
#include "classifier_ptr_set.h"
#include "population.h"
#include "ga.h"
#include "prediction_array.h"
#include "environment.h"
#include "random.h"

template <class Environment, class Symbol, typename Action>
class XCS
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;

    // [P]
    //   The population [P] consists of all classifier that exist in XCS at any time.
    Population<Symbol, Action> m_population;

    // [M]
    //   The match set [M] is formed out of the current [P].
    //   It includes all classifiers that match the current situation.
    ClassifierPtrSet<Symbol, Action> m_matchSet;

    // [A]
    //   The action set [A] is formed out of the current [M].
    //   It includes all classifiers of [M] that propose the executed action.
    ClassifierPtrSet<Symbol, Action> m_actionSet;

    // [A]_-1
    //   The previous action set [A]_-1 is the action set that was active in the last
    //   execution cycle.
    ClassifierPtrSet<Symbol, Action> m_prevActionSet;

    GA<Symbol, Action> m_ga;

    uint64_t m_timeStamp;

    double m_prevReward;

    Situation<Symbol> m_prevSituation;

    const XCSConstants m_constants;

    void generateMatchSet(const Situation<Symbol> & situation)
    {
        m_matchSet.clear();

        while (m_matchSet.empty())
        {
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
            if(!unselectedActions.empty())
            {
                auto coveringClassifier = generateCoveringClassifier(situation, Random::chooseFrom(unselectedActions));
                m_population.insert(coveringClassifier);
                m_population.deleteExtraClassifiers();
                m_matchSet.clear();
            }
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

    auto generateCoveringClassifier(const Situation<Symbol> & situation, Action action) const
    {
        // Generate a more general condition than the situation
        Situation<Symbol> condition(situation);
        condition.randomGeneralize(m_constants.generalizeProbability);

        // Generate a classifier
        return std::make_shared<Classifier<Symbol, Action>>(condition, action, m_timeStamp, m_constants);
    }

    void updateSet(ClassifierPtrSet<Symbol, Action> & actionSet, double p)
    {
        // Calculate numerosity sum used for updating action set size estimate
        uint64_t numerositySum = 0;
        for (auto && cl : actionSet)
        {
            numerositySum += cl->numerosity;
        }

        for (auto && cl : actionSet)
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

        updateFitness(actionSet);

        if (m_constants.doActionSetSubsumption)
        {
            doActionSetSubsumption(actionSet);
        }
    }

    void updateFitness(ClassifierPtrSet<Symbol, Action> & actionSet)
    {
        double accuracySum = 0.0;

        // Accuracy vector
        std::unordered_map<const ClassifierPtr *, double> kappa(std::size(actionSet));

        for (auto && cl : actionSet)
        {
            if (cl->predictionError < m_constants.predictionErrorThreshold)
            {
                kappa[&cl] = 1.0;
            }
            else
            {
                kappa[&cl] = m_constants.alpha * pow(cl->predictionError / m_constants.predictionErrorThreshold, -m_constants.nu);
            }

            accuracySum += kappa[&cl] * cl->numerosity;
        }

        for (auto && cl : actionSet)
        {
            cl->fitness += m_constants.learningRate * (kappa[&cl] * cl->numerosity / accuracySum - cl->fitness);
        }
    }

    void doActionSetSubsumption(ClassifierPtrSet<Symbol, Action> & actionSet)
    {
        ClassifierPtr cl;

        for (auto && c : actionSet)
        {
            if (c->isSubsumer())
            {
                size_t cDontCareCount;
                size_t clDontCareCount;
                if ((cl.get() == nullptr) ||
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
            for (auto && c : actionSet)
            {
                if (cl->isMoreGeneral(*c))
                {
                    cl->numerosity += c->numerosity;
                    removedClassifiers.push_back(&c);
                }
            }

            for (auto && removedClassifier : removedClassifiers)
            {
                m_population.erase(*removedClassifier);
                actionSet.erase(*removedClassifier);
            }
        }
    }

    void runGA(ClassifierPtrSet<Symbol, Action> & actionSet, const Situation<Symbol> & situation)
    {
        uint64_t timeStampNumerositySum = 0;
        uint64_t numerositySum = 0;

        for (auto && cl : actionSet)
        {
            timeStampNumerositySum += cl->timeStamp * cl->numerosity;
            numerositySum += cl->numerosity;
        }

        assert(numerositySum > 0);

        if (m_timeStamp - timeStampNumerositySum / numerositySum > m_constants.thetaGA)
        {
            for (auto && cl : actionSet)
            {
                cl->timeStamp = m_timeStamp;
            }

            m_ga.run(actionSet, situation, m_population);
        }
    }

public:
    Environment environment;

    XCS(const Environment & environment, const XCSConstants & constants)
        : environment(environment),
        m_ga(constants.crossoverProbability, constants.mutationProbability, constants.doGASubsumption, environment.actionChoices),
        m_population(constants.maxPopulationClassifierCount, constants.thetaDel),
        m_constants(constants),
        m_timeStamp(0),
        m_prevReward(0.0)
    {
    }

    void run(uint64_t loopCount)
    {
        // Main loop
        for (uint64_t i = 0; i < loopCount; ++i)
        {
            auto situation = environment.situation();

            generateMatchSet(situation);

            EpsilonGreedyPredictionArray<Symbol, Action> predictionArray(m_matchSet, m_constants.exploreProbability);

            Action action = predictionArray.selectAction();

            generateActionSet(action);

            double reward = environment.executeAction(action);

            if (!m_prevActionSet.empty())
            {
                double p = m_prevReward + m_constants.gamma * predictionArray.max();
                updateSet(m_prevActionSet, p);
                runGA(m_prevActionSet, m_prevSituation);
            }

            if (environment.isEndOfProblem())
            {
                updateSet(m_actionSet, reward);
                runGA(m_actionSet, situation);
                m_prevActionSet.clear();
            }
            else
            {
                m_prevActionSet = m_actionSet;
                m_prevReward = reward;
                m_prevSituation = situation;
            }
            ++m_timeStamp;
        }
    }

    void dumpPopulation() const
    {
        std::cout << "C:A,prediction,epsilon,F,exp,ts,as,n" << std::endl;
        for (auto && cl : m_population)
        {
            std::cout
                << *cl << ","
                << cl->prediction << ","
                << cl->predictionError << ","
                << cl->fitness << ","
                << cl->experience << ","
                << cl->timeStamp << ","
                << cl->actionSetSize << ","
                << cl->numerosity << std::endl;
        }
    }
};
