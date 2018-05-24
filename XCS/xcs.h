#pragma once

#include <string>
#include <list>

#include "symbol.h"
#include "classifier.h"

template <class S = BinarySymbol, class Action = std::string>
class XCS
{
protected:
	std::list<Classifier<S, Action>> m_population; // [P]
	std::list<Classifier<S, Action>> m_matchSet; // [M]
	std::list<Classifier<S, Action>> m_actionSet; // [A]
};