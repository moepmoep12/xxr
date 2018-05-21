#pragma once

#include <list>

#include "classifier.h"

template <class S = BinarySymbol, class Action = std::string>
class ActionSet
{
private:
	std::list<Classifier<S, Action>> m_classifiers;

public:
};