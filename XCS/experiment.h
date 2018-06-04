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
#include "classifier.h"
#include "classifier_ptr_set.h"
#include "population.h"
#include "match_set.h"
#include "action_set.h"
#include "ga.h"
#include "prediction_array.h"
#include "environment.h"
#include "random.h"

namespace xcs
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = Condition<T, Symbol>,
        class Classifier = Classifier<T, Action, Symbol, Condition>,
        class GA = GA<T, Action, Symbol, Condition, Classifier>,
        class Population = Population<T, Action, Symbol, Condition, Classifier>,
        class MatchSet = MatchSet<T, Action, Symbol, Condition, Classifier, Population>,
        class PredictionArray = EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class ActionSet = ActionSet<T, Action, Symbol, Condition, Classifier, GA, Population, MatchSet>,
        class Constants = Constants
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

        std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> m_environment;

    public:
        Experiment(std::shared_ptr<AbstractEnvironment<T, Action, Symbol>> environment, const Constants & constants)
            : m_environment(environment),
            m_population(constants, environment->actionChoices),
            m_matchSet(constants, environment->actionChoices),
            m_actionSet(constants, environment->actionChoices),
            m_prevActionSet(constants, environment->actionChoices),
            m_constants(constants),
            m_timeStamp(0),
            m_prevReward(0.0)
        {
        }

        // RUN EXPERIMENT
        virtual void run(uint64_t loopCount)
        {
            // Main loop
            for (uint64_t i = 0; i < loopCount; ++i)
            {
                auto situation = m_environment->situation();

                m_matchSet.regenerate(m_population, situation, m_timeStamp);

                PredictionArray predictionArray(m_matchSet, m_constants.exploreProbability /* FIXME: for other prediction array than epsilon greedy */ );

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