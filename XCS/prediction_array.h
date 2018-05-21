#pragma once

#include <unordered_map>

#include "match_set.h"

template <class S = BinarySymbol, class Action = std::string>
class PredictionArray
{
private:
	// PA (Prediction Array)
	std::unordered_map<Action, double> m_pa;

public:
	PredictionArray(const MatchSet<S, Action> & matchSet)
	{
		// FSA (Fitness Sum Array)
		std::unordered_map<Action, double> fsa;
	}
};