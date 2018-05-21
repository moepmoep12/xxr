#pragma once

#include <unordered_map>
#include <cmath>

#include "match_set.h"

template <class S = BinarySymbol, class Action = std::string>
class PredictionArray
{
private:
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
};