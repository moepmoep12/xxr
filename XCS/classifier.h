#pragma once

#include <string>
#include <cstdint>

#include "state.h"
#include "xcs_constants.h"

template <class S = BinarySymbol, class Action = std::string>
class Classifier
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
	// Condition (C)
	const State<S> condition;

	// Action (A)
	const Action action;

	Classifier(const State<S> & condition, const Action & action, uint32_t timeStamp, const XCSConstants & constants) :
		condition(condition),
		action(action),
		m_prediction(constants.initialPrediction),
		m_predictionError(constants.initialPredictionError),
		m_fitness(constants.initialFitness),
		m_experience(0),
		m_timeStamp(timeStamp),
		m_actionSetSize(1),
		m_numerosity(1),
		m_constants(constants)
		{}

	bool equals(const Classifier<S, Action> & classifier) const
	{
		return condition == classifier.condition && action == classifier.action;
	}

	bool isMoreGeneral(const Classifier<S, Action> & classifier) const
	{
		return condition.contains(classifier) && condition != classifier.condition;
	}

	bool isSubsumer() const
	{
		return m_experience > m_constants.thetaSub && m_predictionError < m_constants.predictionErrorThreshold;
	}

	bool subsumes(const Classifier<S, Action> & classifier) const
	{
		return isSubsumer() && isMoreGeneral(classifier);
	}

	friend std::ostream & operator<< (std::ostream & os, const Classifier<S, Action> & obj)
	{
		return os << obj.condition << ":" << obj.action;
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