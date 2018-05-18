#pragma once

#include <string>
#include <cstdint>

#include "state.h"

template <S = BinarySymbol, Action = std::string>
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
	explicit Classifier(const State<S> & condition, const Action & action, uint32_t timeStamp)
		: m_condition(condition), m_action(action), m_prediction(0/* FIXME: P_I */), m_predictionError(0/* FIXME: epsilon_I */),
		m_fitness(0/* FIXME: F_I */), m_experience(0), m_timeStamp(timeStamp), m_actionSetSize(1), m_numerosity(1) {}

	bool contains(const Classifier<S, Action> & classifier) const
	{
		assert(std::size(m_condition) == std::size(classifier.m_symbols));

		for (size_t i = 0; i < std::size(m_symbols); ++i)
		{
			if (!m_symbols[i].contains(classifier.m_symbols[i]))
			{
				return false;
			}
		}

		return true;
	}
};