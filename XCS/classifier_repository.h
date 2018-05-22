#pragma once

#include <list>
#include <unordered_map>
#include <cstddef>

#include "symbol.h"
#include "classifier.h"

template <class S = BinarySymbol, class Action = std::string>
class ClassifierRepository
{
private:
	std::list<Classifier<S, Action>> m_classifiers;
    std::unordered_map<Action, size_t> m_actionClassifierCounts;
    size_t m_actionCount;

protected:
    void addClassifier(const Classifier<S, Action> & cl)
    {
        m_classifiers.push_back(cl);

        if (++m_actionClassifierCounts[cl.action] == 1)
        {
            ++m_actionCount;
        }
    }

    void eraseClassifier(decltype(m_classifiers)::iterator itr)
    {
        m_classifiers.erase(itr);

        if (--m_actionClassifierCounts[cl.action] == 0)
        {
            --m_actionCount;
        }
    }

public:
	~ClassifierRepository() = 0;

	const auto & classifiers() const
	{
		return m_classifiers;
	}
};