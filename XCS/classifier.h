#pragma once

#include <string>
#include <cstdint>

#include "situation.h"
#include "xcs_constants.h"

template <class Symbol, typename Action>
struct ConditionActionPair
{
    // C
    //   The condition specifies the input states (sensory situations)
    //   in which the classifier can be applied (matches).
    const Situation<Symbol> condition;

    // A
    //   The action specifies the action (possibly a clasification)
    //   that the classifier proposes.
    const Action action;

    bool equals(const ConditionActionPair<Symbol, Action> & cl) const
    {
        return condition == cl.condition && action == cl.action;
    }

    bool isMoreGeneral(const ConditionActionPair<Symbol, Action> & cl) const
    {
        return condition.contains(cl) && condition != cl.condition;
    }

    friend std::ostream & operator<< (std::ostream & os, const ConditionActionPair<Symbol, Action> & obj)
    {
        return os << obj.condition << ":" << obj.action;
    }
};

template <class Symbol, typename Action>
class Classifier : public ConditionActionPair<Symbol, Action>
{
private:
    // p
    //   The prediction p estimates (keeps an average of) the payoff expected if the
    //   classifier matches and its action is taken by the system.
    double m_prediction;

    // epsilon
    //   The prediction error epsilon estimates the errors made in the predictions.
    double m_predictionError;

    // F
    //   The fitness F denotes the classifier's fitness.
    double m_fitness;

    // exp
    //   The experience exp counts the number of times since its creation that the
    //   classifier has belonged to an action set.
    double m_experience;

    // ts
    //   The time stamp ts denotes the time-step of the last occurrence of a GA in
    //   an action set to which this classifier belonged.
    uint64_t m_timeStamp;

    // as
    //   The action set size as estimates the average size of the action sets this
    //   classifier has belonged to.
    double m_actionSetSize;

    // n
    //   The numerosity n reflects the number of micro-classifiers (ordinary
    //   classifiers) this classifier - which is technically called a macro-
    //   classifier - represents.
    uint64_t m_numerosity;

    // Constants
    const double m_thetaSub;
    const double m_predictionErrorThreshold;

public:
    Classifier(const Classifier<Symbol, Action> & obj) :
        ConditionActionPair<Symbol, Action>{ obj.condition, obj.action },
        m_prediction(obj.m_prediction),
        m_predictionError(obj.m_predictionError),
        m_fitness(obj.m_fitness),
        m_experience(obj.m_experience),
        m_timeStamp(obj.m_timeStamp),
        m_actionSetSize(obj.m_actionSetSize),
        m_numerosity(obj.m_numerosity),
        m_thetaSub(obj.m_thetaSub),
        m_predictionErrorThreshold(obj.m_predictionErrorThreshold)
    {
    }

    Classifier(const Situation<Symbol> & condition, Action action, uint64_t timeStamp, const XCSConstants & constants) :
        ConditionActionPair<Symbol, Action>{ condition, action },
        m_prediction(constants.initialPrediction),
        m_predictionError(constants.initialPredictionError),
        m_fitness(constants.initialFitness),
        m_experience(0),
        m_timeStamp(timeStamp),
        m_actionSetSize(1),
        m_numerosity(1),
        m_thetaSub(constants.thetaSub),
        m_predictionErrorThreshold(constants.predictionErrorThreshold)
    {
    }

    Classifier(const std::string & condition, Action action, uint64_t timeStamp, const XCSConstants & constants) :
        Classifier(Situation<Symbol>(condition), action, timeStamp, constants)
    {
    }

    bool isSubsumer() const
    {
        return m_experience > m_thetaSub && m_predictionError < m_predictionErrorThreshold;
    }

    bool subsumes(const Classifier<Symbol, Action> & cl) const
    {
        return isSubsumer() && isMoreGeneral(cl);
    }

    auto prediction() const
    {
        return m_prediction;
    }

    auto fitness() const
    {
        return m_fitness;
    }

    auto experience() const
    {
        return m_experience;
    }

    auto timeStamp() const
    {
        return m_timeStamp;
    }

    auto actionSetSize() const
    {
        return m_actionSetSize;
    }

    auto numerosity() const
    {
        return m_numerosity;
    }

    void decreaseNumerosity()
    {
        --m_numerosity;
    }

    Classifier<Symbol, Action> & operator= (const Classifier<Symbol, Action> &) { return *this; };
};