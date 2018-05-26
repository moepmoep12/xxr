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

    virtual ~ConditionActionPair() = default;

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
struct Classifier : ConditionActionPair<Symbol, Action>
{
    // p
    //   The prediction p estimates (keeps an average of) the payoff expected if the
    //   classifier matches and its action is taken by the system.
    double prediction;

    // epsilon
    //   The prediction error epsilon estimates the errors made in the predictions.
    double predictionError;

    // F
    //   The fitness F denotes the classifier's fitness.
    double fitness;

    // exp
    //   The experience exp counts the number of times since its creation that the
    //   classifier has belonged to an action set.
    double experience;

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

private:
    // Constants
    const double m_thetaSub;
    const double m_predictionErrorThreshold;

public:
    Classifier(const Classifier<Symbol, Action> & obj) :
        ConditionActionPair<Symbol, Action>{ obj.condition, obj.action },
        prediction(obj.prediction),
        predictionError(obj.predictionError),
        fitness(obj.fitness),
        experience(obj.experience),
        timeStamp(obj.timeStamp),
        actionSetSize(obj.actionSetSize),
        numerosity(obj.numerosity),
        m_thetaSub(obj.m_thetaSub),
        m_predictionErrorThreshold(obj.m_predictionErrorThreshold)
    {
    }

    Classifier(const Situation<Symbol> & condition, Action action, uint64_t timeStamp, const XCSConstants & constants) :
        ConditionActionPair<Symbol, Action>{ condition, action },
        prediction(constants.initialPrediction),
        predictionError(constants.initialPredictionError),
        fitness(constants.initialFitness),
        experience(0),
        timeStamp(timeStamp),
        actionSetSize(1),
        numerosity(1),
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
        return experience > m_thetaSub && predictionError < m_predictionErrorThreshold;
    }

    bool subsumes(const Classifier<Symbol, Action> & cl) const
    {
        return isSubsumer() && isMoreGeneral(cl);
    }

    Classifier<Symbol, Action> & operator= (const Classifier<Symbol, Action> &) { return *this; };
};