#pragma once

#include <iostream>
#include <sstream>
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

        // Available action choices
        std::unordered_set<Action> m_availableActions;

        uint64_t m_timeStamp;

        bool m_expectsReward;
        double m_prevReward;

        std::vector<T> m_prevSituation;

        const Constants m_constants;

    public:
        // Constructor
        Experiment(const std::unordered_set<Action> & availableActions, const Constants & constants) :
            m_population(constants, availableActions),
            m_matchSet(constants, availableActions),
            m_actionSet(constants, availableActions),
            m_prevActionSet(constants, availableActions),
            m_availableActions(availableActions),
            m_timeStamp(0),
            m_expectsReward(false),
            m_prevReward(0.0),
            m_constants(constants)
        {
        }

        // Destructor
        virtual ~Experiment() = default;

        // Run with exploration
        virtual Action explore(const std::vector<T> & situation)
        {
            assert(!m_expectsReward);

            m_matchSet.regenerate(m_population, situation, m_timeStamp);

            PredictionArray predictionArray(m_matchSet, m_constants.exploreProbability);

            Action action = predictionArray.selectAction();

            m_actionSet.regenerate(m_matchSet, action);

            m_expectsReward = true;

            if (!m_prevActionSet.empty())
            {
                double p = m_prevReward + m_constants.gamma * predictionArray.max();
                m_prevActionSet.update(p, m_population);
                m_prevActionSet.runGA(m_prevSituation, m_population, m_timeStamp);
            }

            m_prevSituation = situation;

            return action;
        }

        virtual void reward(double value, bool isEndOfProblem = true)
        {
            assert(m_expectsReward);

            if (isEndOfProblem)
            {
                m_actionSet.update(value, m_population);
                m_actionSet.runGA(m_prevSituation, m_population, m_timeStamp);
                m_prevActionSet.clear();
            }
            else
            {
                m_actionSet.copyTo(m_prevActionSet);
                m_prevReward = value;
            }
            ++m_timeStamp;

            m_expectsReward = false;
        }

        // Run without exploration
        virtual Action exploit(const std::vector<T> & situation) const
        {
            // Create new match set as sandbox
            MatchSet matchSet(m_constants, m_availableActions);
            for (auto && cl : m_population)
            {
                if (cl->condition.matches(situation))
                {
                    matchSet.insert(cl);
                }
            }

            if (!matchSet.empty())
            {
                auto predictionArray = GreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>(matchSet);
                return predictionArray.selectAction();
            }
            else
            {
                return Random::chooseFrom(m_availableActions);
            }
        }

        virtual std::string dumpPopulation() const
        {
            std::stringstream ss;
            ss << "Condition,Action,prediction,epsilon,F,exp,ts,as,n\n";
            for (auto && cl : m_population)
            {
                ss  << cl->condition << ","
                    << cl->action << ","
                    << cl->prediction << ","
                    << cl->predictionError << ","
                    << cl->fitness << ","
                    << cl->experience << ","
                    << cl->timeStamp << ","
                    << cl->actionSetSize << ","
                    << cl->numerosity << "\n";
            }
            return ss.str();
        }
        
        virtual std::size_t populationSize() const
        {
            return m_population.size();
        }
    };

}
