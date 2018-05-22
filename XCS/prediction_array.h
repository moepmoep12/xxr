#pragma once

#include <unordered_map>
#include <unordered_set>
#include <random>
#include <limits>
#include <cfloat>
#include <cmath>

#include "match_set.h"
#include "random.h"

template <class S = BinarySymbol, class Action = std::string>
class PredictionArray
{
protected:
	// PA (Prediction Array)
	std::unordered_map<Action, double> m_pa;

	// Array of PA keys (for random action selection)
	std::vector<Action> m_paActions;

	// The best actions of PA
	std::vector<Action> m_bestPAActions;

public:
	explicit PredictionArray(const MatchSet<S, Action> & matchSet)
	{
		// FSA (Fitness Sum Array)
		std::unordered_map<Action, double> fsa;

		for (auto && cl : matchSet.classifiers())
		{
			if (m_pa.count(cl.action) == 0) {
				m_paActions.push_back(cl.action);
			}
			m_pa[cl.action] += cl.prediction() * cl.fitness();
			fsa[cl.action] += cl.fitness();
		}

		double max = numeric_limits<double>::lowest();

		for (auto && pair : m_pa)
		{
			if (fabs(fsa[pair.first]) > 0.0)
			{
				pair.second /= fsa[pair.first];
			}

			// Update the best actions
			if (fabs(max - pair.second) < DBL_EPSILON) // max == pair.second
			{
				m_paActions.push_back(pair.first);
			}
			else if (max < pair.second)
			{
				m_paActions.clear();
				m_paActions.push_back(pair.first);
				max = pair.second;
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
		if (Random::nextDouble() < m_epsilon)
			return Random::chooseFrom(m_paActions); // Choose random action
		else
			return Random::chooseFrom(m_bestPAActions); // Choose best action
	}
};
