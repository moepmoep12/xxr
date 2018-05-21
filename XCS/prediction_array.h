#pragma once

#include <unordered_map>
#include <random>
#include <cmath>

#include "match_set.h"
#include "random.h"

template <class S = BinarySymbol, class Action = std::string>
class PredictionArray
{
protected:
	// PA (Prediction Array)
	std::unordered_map<Action, double> m_pa;

public:
	explicit PredictionArray(const MatchSet<S, Action> & matchSet)
	{
		// FSA (Fitness Sum Array)
		std::unordered_map<Action, double> fsa;

		for (auto && cl : matchSet.classifiers())
		{
			m_pa[cl.action] += cl.prediction() * cl.fitness();
			fsa[cl.action] += cl.fitness();
		}

		for (auto && pair : m_pa)
		{
			if (fabs(fsa[pair.first]) > 0.0)
			{
				pair.second /= fsa[pair.first];
			}
		}
	}

	Action selectAction() = 0;
};

template <class S = BinarySymbol, class Action = std::string>
class EpsilonGreedyPredictionArray : public PredictonArray<S, Action>
{
private:
	double m_epsilon;
public:
	EpsilonGreedyPredictionArray(double epsilon, const MatchSet<S, Action> & matchSet)
		: PredictionArray(matchSet), m_epsilon(epsilon) {}

	Action selectAction()
	{
		// Random selection
		if (Random::nextDouble())
		{
			
		}
	}
};
