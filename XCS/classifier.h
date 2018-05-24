#pragma once

#include <cstdint>

#include "state.h"
#include "xcs_constants.h"

template <class S = BinarySymbol, typename Action = int>
struct ConditionActionPair
{
	// Condition (C)
	const State<S> condition;

	// Action (A)
	const Action action;

	bool equals(const ConditionActionPair<S, Action> & cl) const
	{
		return condition == cl.condition && action == cl.action;
	}

	bool isMoreGeneral(const ConditionActionPair<S, Action> & cl) const
	{
		return condition.contains(cl) && condition != cl.condition;
	}

	friend std::ostream & operator<< (std::ostream & os, const ConditionActionPair<S, Action> & obj)
	{
		return os << obj.condition << ":" << obj.action;
	}
};

template <class S = BinarySymbol, typename Action = int>
class Classifier : public ConditionActionPair<S, Action>
{
private:
	// Prediction (p)
	double m_prediction;

	// Prediction error (epsilon)
	double m_predictionError;

	// Fitness (f)
	double m_fitness;

	// Experience (exp)
	double m_experience;

	// Time stamp (ts)
	uint32_t m_timeStamp;

	// Action set size (as)
	double m_actionSetSize;

	// Numerosity (num)
	uint32_t m_numerosity;

	// Learning parameters
	const XCSConstants m_constants;

public:
	Classifier(const State<S> & condition, Action action, uint32_t timeStamp, const XCSConstants & constants) :
		ConditionActionPair{condition, action},
		m_prediction(constants.initialPrediction),
		m_predictionError(constants.initialPredictionError),
		m_fitness(constants.initialFitness),
		m_experience(0),
		m_timeStamp(timeStamp),
		m_actionSetSize(1),
		m_numerosity(1),
		m_constants(constants)
		{}

	bool isSubsumer() const
	{
		return m_experience > m_constants.thetaSub && m_predictionError < m_constants.predictionErrorThreshold;
	}

	bool subsumes(const Classifier<S, Action> & cl) const
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
};