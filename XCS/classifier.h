#pragma once

#include <string>
#include <cstdint>

#include "state.h"
#include "xcs_constants.h"

template <class S = BinarySymbol, class Action = std::string>
class Classifier
{
private:
	// Condition (C)
	State<S> m_condition;

	// Action (A)
	Action m_action;

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
	
public:
	Classifier(const State<S> & condition, const Action & action, uint32_t timeStamp, const XCSConstants & constants) :
		m_condition(condition),
		m_action(action),
		m_prediction(constants.initialPrediction),
		m_predictionError(constants.initialPredictionError),
		m_fitness(constants.initialFitness),
		m_experience(0),
		m_timeStamp(timeStamp),
		m_actionSetSize(1),
		m_numerosity(1)
		{}

	bool equals(const Classifier<S, Action> & classifier) const
	{
		return m_condition == classifier.m_condition && m_action == classifier.m_action;
	}

	bool isMoreGeneral(const Classifier<S, Action> & classifier) const
	{
		return m_condition.contains(classifier) && m_condition != classifier.m_condition;
	}

	friend std::ostream & operator<< (std::ostream & os, const Classifier<S, Action> & obj)
	{
		return os << obj.m_condition << ":" << obj.m_action;
	}
};