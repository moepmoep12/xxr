#pragma once

#include <list>

#include "symbol.h"
#include "classifier.h"

template <class S = BinarySymbol, class Action = std::string>
class ClassifierRepository
{
protected:
	std::list<Classifier<S, Action>> m_classifiers;

public:
	std::list<Classifier<S, Action>> classifiers() const
	{
		return m_classifiers;
	}
};