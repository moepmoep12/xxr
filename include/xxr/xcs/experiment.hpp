#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cmath>

#include "../experiment.hpp"
#include "constants.hpp"
#include "symbol.hpp"
#include "condition.hpp"
#include "classifier.hpp"
#include "classifier_ptr_set.hpp"
#include "population.hpp"
#include "match_set.hpp"
#include "action_set.hpp"
#include "ga.hpp"
#include "prediction_array.hpp"
#include "../random.hpp"

namespace xxr { namespace xcs_impl
{

    template <
        typename T,
        typename Action,
        class PredictionArray = EpsilonGreedyPredictionArray<
            MatchSet<
                Population<
                    ClassifierPtrSet<
                        Classifier<
                            ConditionActionPair<Condition<Symbol<T>>, Action>,
                            Constants
                        >
                    >
                >
            >
        >,
        class ActionSet = ActionSet<
            GA<
                Population<
                    ClassifierPtrSet<
                        Classifier<
                            ConditionActionPair<Condition<Symbol<T>>, Action>,
                            Constants
                        >
                    >
                >
            >
        >
    >
    class Experiment : public AbstractExperiment<T, Action>
    {
    public:
        static_assert(
            std::is_same<typename PredictionArray::PopulationType, typename ActionSet::PopulationType>::value,
            "PredictionArray::PopulationType and ActionSet::PopulationType must not be different");
        static_assert(
            std::is_same<T, typename PredictionArray::type>::value,
            "Experiment::type and PredictionArray::type must not be different");
        static_assert(
            std::is_same<T, typename ActionSet::type>::value,
            "Experiment::type and ActionSet::type must not be different");
        static_assert(
            std::is_same<Action, typename PredictionArray::ActionType>::value,
            "Experiment::ActionType and PredictionArray::ActionType must not be different");
        static_assert(
            std::is_same<Action, typename ActionSet::ActionType>::value,
            "Experiment::ActionType and ActionSet::ActionType must not be different");

        using type = T;
        using SymbolType = typename PredictionArray::SymbolType;
        using ConditionType = typename PredictionArray::ConditionType;
        using ConditionActionPairType = typename PredictionArray::ConditionActionPairType;
        using ConstantsType = typename PredictionArray::ConstantsType;
        using ClassifierType = typename PredictionArray::ClassifierType;
        using ClassifierPtrSetType = typename PredictionArray::ClassifierPtrSetType;
        using PopulationType = typename PredictionArray::PopulationType;
        using MatchSetType = typename PredictionArray::MatchSetType;
        using PredictionArrayType = PredictionArray;
        using GAType = typename ActionSet::GAType;
        using ActionSetType = ActionSet;

        ConstantsType constants;

    protected:
        using ClassifierPtr = std::shared_ptr<ClassifierType>;

        // [P]
        //   The population [P] consists of all classifier that exist in XCS at any time.
        PopulationType m_population;

        // [M]
        //   The match set [M] is formed out of the current [P].
        //   It includes all classifiers that match the current situation.
        MatchSetType m_matchSet;

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
        bool m_isPrevModeExplore;

        std::vector<T> m_prevSituation;

    public:
        // Constructor
        Experiment(const std::unordered_set<Action> & availableActions, const ConstantsType & constants) :
            constants(constants),
            m_population(this->constants, availableActions),
            m_matchSet(this->constants, availableActions),
            m_actionSet(this->constants, availableActions),
            m_prevActionSet(this->constants, availableActions),
            m_availableActions(availableActions),
            m_timeStamp(0),
            m_expectsReward(false),
            m_prevReward(0.0),
            m_isPrevModeExplore(false)
        {
        }

        // Destructor
        virtual ~Experiment() = default;

        // Run with exploration
        virtual Action explore(const std::vector<T> & situation) override
        {
            assert(!m_expectsReward);

            m_matchSet.regenerate(m_population, situation, m_timeStamp);

            PredictionArray predictionArray(m_matchSet, constants.exploreProbability);

            Action action = predictionArray.selectAction();

            m_actionSet.regenerate(m_matchSet, action);

            m_expectsReward = true;
            m_isPrevModeExplore = true;

            if (!m_prevActionSet.empty())
            {
                double p = m_prevReward + constants.gamma * predictionArray.max();
                m_prevActionSet.update(p, m_population);
                m_prevActionSet.runGA(m_prevSituation, m_population, m_timeStamp);
            }

            m_prevSituation = situation;

            return action;
        }

        virtual void reward(double value, bool isEndOfProblem = true) override
        {
            assert(m_expectsReward);

            if (isEndOfProblem)
            {
                m_actionSet.update(value, m_population);
                if (m_isPrevModeExplore) // Do not perform GA operations in exploitation
                {
                    m_actionSet.runGA(m_prevSituation, m_population, m_timeStamp);
                }
                m_prevActionSet.clear();
            }
            else
            {
                m_actionSet.copyTo(m_prevActionSet);
                m_prevReward = value;
            }

            if (m_isPrevModeExplore) // Do not increment actual time in exploitation
            {
                ++m_timeStamp;
            }

            m_expectsReward = false;
        }

        // Run without exploration
        // (Set update to true when testing multi-step problems. If update is true, make sure to call reward() after this.)
        virtual Action exploit(const std::vector<T> & situation, bool update = false) override
        {
            if (update)
            {
                assert(!m_expectsReward);

                m_matchSet.regenerate(m_population, situation, m_timeStamp);

                GreedyPredictionArray<MatchSetType> predictionArray(m_matchSet);

                Action action = predictionArray.selectAction();

                m_actionSet.regenerate(m_matchSet, action);

                m_expectsReward = true;
                m_isPrevModeExplore = false;

                if (!m_prevActionSet.empty())
                {
                    double p = m_prevReward + constants.gamma * predictionArray.max();
                    m_prevActionSet.update(p, m_population);

                    // Do not perform GA operations in exploitation
                }

                m_prevSituation = situation;

                return action;
            }
            else
            {
                // Create new match set as sandbox /*(because of const member function)*/
                MatchSetType matchSet(constants, m_availableActions);
                for (auto && cl : m_population)
                {
                    if (cl->condition.matches(situation))
                    {
                        matchSet.insert(cl);
                    }
                }

                if (!matchSet.empty())
                {
                    GreedyPredictionArray<MatchSetType> predictionArray(matchSet);
                    return predictionArray.selectAction();
                }
                else
                {
                    return Random::chooseFrom(m_availableActions);
                }
            }
        }

        virtual std::string dumpPopulation() const override
        {
            std::stringstream ss;
            ss << "Condition,Action,prediction,epsilon,F,exp,ts,as,n,acc\n";
            for (auto && cl : m_population)
            {
                ss  << cl->condition << ","
                    << cl->action << ","
                    << cl->prediction << ","
                    << cl->epsilon << ","
                    << cl->fitness << ","
                    << cl->experience << ","
                    << cl->timeStamp << ","
                    << cl->actionSetSize << ","
                    << cl->numerosity << ","
                    << cl->accuracy() << "\n";
            }
            return ss.str();
        }

        virtual std::size_t populationSize() const override
        {
            return m_population.size();
        }

        virtual void switchToCondensationMode() noexcept override
        {
            constants.chi = 0.0;
            constants.mu = 0.0;
        }
    };

}}
