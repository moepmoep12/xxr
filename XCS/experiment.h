#pragma once

#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cmath>

#include "constants.h"
#include "symbol.h"
#include "condition.h"
#include "classifier.h"
#include "classifier_ptr_set.h"
#include "population.h"
#include "match_set.h"
#include "action_set.h"
#include "ga.h"
#include "prediction_array.h"
#include "environment.h"
#include "random.h"

namespace XCS
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = Condition<T, Symbol>,
        class ConditionActionPair = ConditionActionPair<T, Action, Symbol, Condition>,
        class Constants = Constants,
        class Classifier = Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>,
        class ClassifierPtrSet = ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = Population<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>,
        class ActionSet = ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;

        // [P]
        //   The population [P] consists of all classifier that exist in XCS at any time.
        Population m_population;

        // [M]
        //   The match set [M] is formed out of the current [P].
        //   It includes all classifiers that match the current situation.
        MatchSet m_matchSet;

        // [A]
        //   The action set [A] is formed out of the current [M].
        //   It includes all classifiers of [M] that propose the executed action.
        ActionSet m_actionSet;

        // [A]_-1
        //   The previous action set [A]_-1 is the action set that was active in the last
        //   execution cycle.
        ActionSet m_prevActionSet;

        uint64_t m_timeStamp;

        double m_prevReward;

        std::vector<T> m_prevSituation;

        const Constants m_constants;

        // Environment(s)
        std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> m_environment;
        std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> m_evaluationEnvironment;

    public:
        // Constructor
        Experiment(std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> environment, const Constants & constants) :
            m_population(constants, environment->availableActions),
            m_matchSet(constants, environment->availableActions),
            m_actionSet(constants, environment->availableActions),
            m_prevActionSet(constants, environment->availableActions),
            m_timeStamp(0),
            m_prevReward(0.0),
            m_constants(constants),
            m_environment(environment),
            m_evaluationEnvironment(environment)
        {
        }

        // Use this if the environment situation can be changed by the action execution history
        Experiment(std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> environment, std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> evaluationEnvironment, const Constants & constants) :
            m_environment(environment),
            m_evaluationEnvironment(evaluationEnvironment),
            m_population(constants, environment->availableActions),
            m_matchSet(constants, environment->availableActions),
            m_actionSet(constants, environment->availableActions),
            m_prevActionSet(constants, environment->availableActions),
            m_constants(constants),
            m_timeStamp(0),
            m_prevReward(0.0)
        {
            assert(environment->availableActions == evaluationEnvironment->availableActions);
        }

        // Destructor
        virtual ~Experiment() = default;

        // RUN EXPERIMENT
        virtual void run(std::size_t loopCount)
        {
            // Main loop
            for (std::size_t i = 0; i < loopCount; ++i)
            {
                auto situation = m_environment->situation();

                m_matchSet.regenerate(m_population, situation, m_timeStamp);

                PredictionArray predictionArray(m_matchSet, m_constants.exploreProbability);

                Action action = predictionArray.selectAction();

                m_actionSet.regenerate(m_matchSet, action);

                double reward = m_environment->executeAction(action);

                if (!m_prevActionSet.empty())
                {
                    double p = m_prevReward + m_constants.gamma * predictionArray.max();
                    m_prevActionSet.update(p, m_population);
                    m_prevActionSet.runGA(m_prevSituation, m_population, m_timeStamp);
                }

                if (m_environment->isEndOfProblem())
                {
                    m_actionSet.update(reward, m_population);
                    m_actionSet.runGA(situation, m_population, m_timeStamp);
                    m_prevActionSet.clear();
                }
                else
                {
                    m_actionSet.copyTo(m_prevActionSet);
                    m_prevReward = reward;
                    m_prevSituation = situation;
                }
                ++m_timeStamp;
            }
        }

        // Runs experiment without exploration and returns reward average
        virtual double evaluate(std::size_t loopCount) const
        {
            double rewardSum = 0.0;
            for (std::size_t i = 0; i < loopCount; ++i)
            {
                auto situation = m_evaluationEnvironment->situation();

                MatchSet matchSet(m_constants, m_environment->availableActions);
                for (auto && cl : m_population)
                {
                    if (cl->condition.matches(situation))
                    {
                        matchSet.insert(cl);
                    }
                }
                
                Action action;
                if (!matchSet.empty())
                {
                    auto predictionArray = GreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>(matchSet);
                    action = predictionArray.selectAction();
                }
                else
                {
                    action = Random::chooseFrom(m_environment->availableActions);
                }

                rewardSum += m_environment->executeAction(action);
            }

            return rewardSum / loopCount;
        }

        virtual void dumpPopulation() const
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

}
