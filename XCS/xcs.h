#pragma once

#include <vector>
#include <list>
#include <unordered_set>

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

	void generateMatchList(std::list<Classifier<S, Action>> & matchList, const State<S> & situation)
	{
		matchList.clear();

		auto availableActionChoices = m_actionChoices;

		for (auto && cl : m_population)
		{
			if (cl.condition.contains(situation))
			{
				matchList.push_back(cl);
				availableActionChoices.erase(cl.action);
			}
		}

		// Covering
		for (Action action : availableActionChoices)
		{

		}
	}

public:
	explicit XCS(const XCSConstants & constants, const std::set<Action> & actionChoices) : m_constants(constants), m_actionChoices(actionChoices) {}
};