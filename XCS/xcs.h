#pragma once

#include <vector>
#include <list>
#include <unordered_set>
#include <cstdint>

#include "xcs_constants.h"
#include "symbol.h"
#include "classifier.h"
#include "random.h"

template <class S = BinarySymbol, typename Action = int>
class XCS
{
protected:
	std::list<Classifier<S, Action>> m_population; // [P]

	XCSConstants m_constants;

	std::unordered_set<Action> m_actionChoices;

	uint64_t m_timeStamp;

	void generateMatchSet(std::list<Classifier<S, Action>> & matchSet, const State<S> & situation)
	{
		matchSet.clear();

		auto unselectedActions = m_actionChoices;

		for (auto && cl : m_population)
		{
			if (cl.condition.contains(situation))
			{
				matchSet.push_back(cl);
				unselectedActions.erase(cl.action);
			}
		}

		// Generate classifiers covering unselected actions
		for (Action action : unselectedActions)
		{
			auto coveringClassifier = generateCoveringClassifier(situation, action);
			m_population.push_back(coveringClassifier);
			// TODO: DEFETE FROM POPULATION [P]
			matchSet.push_back(coveringClassifier);
		}
	}

	auto generateCoveringClassifier(const State<S> & situation, Action action)
	{
		// Generate more general condition than given situation
		auto condition = situation;
		condition.randomGeneralize(m_constants.generalizeProbability);

		// Generate classifier
		Classifier<S, Action> cl(condition, action, m_timeStamp);
		return cl;
	}

public:
	explicit XCS(const XCSConstants & constants, const std::set<Action> & actionChoices) : m_constants(constants), m_actionChoices(actionChoices), m_timeStamp(0) {}
};