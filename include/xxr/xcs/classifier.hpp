#pragma once

#include <string>
#include <unordered_set>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <cassert>

namespace xxr { namespace xcs_impl
{

    template <typename Action, class Symbol, class Condition>
    struct ConditionActionPair
    {
        // C
        //   The condition specifies the input states (sensory situations)
        //   in which the classifier can be applied (matches).
        Condition condition;

        // A
        //   The action specifies the action (possibly a classification)
        //   that the classifier proposes.
        Action action;

        // Constructor
        ConditionActionPair(const Condition & condition, Action action) : condition(condition), action(action) {}

        ConditionActionPair(Condition && condition, Action action) : condition(std::move(condition)), action(action) {}

        // Destructor
        virtual ~ConditionActionPair() = default;

        // IS MORE GENERAL
        virtual bool isMoreGeneral(const ConditionActionPair<Action, Symbol, Condition> & cl) const
        {
            assert(condition.size() == cl.condition.size());

            if (condition.dontCareCount() <= cl.condition.dontCareCount())
            {
                return false;
            }

            for (std::size_t i = 0; i < condition.size(); ++i)
            {
                if (!condition.at(i).isDontCare() && condition.at(i) != cl.condition.at(i))
                {
                    return false;
                }
            }

            return true;
        }

        friend std::ostream & operator<< (std::ostream & os, const ConditionActionPair<Action, Symbol, Condition> & obj)
        {
            return os << obj.condition << ":" << obj.action;
        }
    };

    template <typename Action, class Symbol, class Condition, class ConditionActionPair, class Constants>
    struct Classifier : ConditionActionPair
    {
        // p
        //   The prediction p estimates (keeps an average of) the payoff expected if the
        //   classifier matches and its action is taken by the system.
        double prediction;

        // epsilon
        //   The prediction error epsilon estimates the errors made in the predictions.
        double epsilon;

        // F
        //   The fitness F denotes the classifier's fitness.
        double fitness;

        // exp
        //   The experience exp counts the number of times since its creation that the
        //   classifier has belonged to an action set.
        uint64_t experience;

        // ts
        //   The time stamp ts denotes the time-step of the last occurrence of a GA in
        //   an action set to which this classifier belonged.
        uint64_t timeStamp;

        // as
        //   The action set size as estimates the average size of the action sets this
        //   classifier has belonged to.
        double actionSetSize;

        // n
        //   The numerosity n reflects the number of micro-classifiers (ordinary
        //   classifiers) this classifier - which is technically called a macro-
        //   classifier - represents.
        uint64_t numerosity;

        using ConditionActionPair::action;
        using ConditionActionPair::isMoreGeneral;

    protected:
        // Constants
        Constants & m_constants;

    public:
        // Constructor
        Classifier(const Classifier & obj) :
            ConditionActionPair(obj.condition, obj.action),
            prediction(obj.prediction),
            epsilon(obj.epsilon),
            fitness(obj.fitness),
            experience(obj.experience),
            timeStamp(obj.timeStamp),
            actionSetSize(obj.actionSetSize),
            numerosity(obj.numerosity),
            m_constants(obj.m_constants)
        {
        }

        Classifier(const Condition & condition, Action action, uint64_t timeStamp, Constants & constants) :
            ConditionActionPair(condition, action),
            prediction(constants.initialPrediction),
            epsilon(constants.initialEpsilon),
            fitness(constants.initialFitness),
            experience(0),
            timeStamp(timeStamp),
            actionSetSize(1),
            numerosity(1),
            m_constants(constants)
        {
        }

        Classifier(const ConditionActionPair & conditionActionPair, uint64_t timeStamp, Constants & constants) :
            ConditionActionPair(conditionActionPair),
            prediction(constants.initialPrediction),
            epsilon(constants.initialEpsilon),
            fitness(constants.initialFitness),
            experience(0),
            timeStamp(timeStamp),
            actionSetSize(1),
            numerosity(1),
            m_constants(constants)
        {
        }

        Classifier(ConditionActionPair && conditionActionPair, uint64_t timeStamp, Constants & constants) :
            ConditionActionPair(std::move(conditionActionPair)),
            prediction(constants.initialPrediction),
            epsilon(constants.initialEpsilon),
            fitness(constants.initialFitness),
            experience(0),
            timeStamp(timeStamp),
            actionSetSize(1),
            numerosity(1),
            m_constants(constants)
        {
        }

        Classifier(const std::vector<typename Symbol::type> & situation, Action action, uint64_t timeStamp, Constants & constants) :
            Classifier(Condition(situation), action, timeStamp, constants)
        {
        }

        Classifier(const std::string & condition, Action action, uint64_t timeStamp, Constants & constants) :
            Classifier(Condition(condition), action, timeStamp, constants)
        {
        }

        // Destructor
        virtual ~Classifier() = default;

        // COULD SUBSUME
        virtual bool isSubsumer() const
        {
            return experience > m_constants.thetaSub && epsilon < m_constants.epsilonZero;
        }

        // DOES SUBSUME
        virtual bool subsumes(const Classifier<Action, Symbol, Condition, ConditionActionPair, Constants> & cl) const
        {
            return action == cl.action && isSubsumer() && isMoreGeneral(cl);
        }

        virtual double accuracy() const
        {
            if (epsilon < m_constants.epsilonZero)
            {
                return 1.0;
            }
            else
            {
                return m_constants.alpha * pow(epsilon / m_constants.epsilonZero, -m_constants.nu);
            }
        }
    };

}}
