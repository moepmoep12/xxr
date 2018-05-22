#pragma once

#include "symbol.h"
#include "population.h"
#include "state.h"
#include "classifier.h"
#include "classifier_repository.h"

template <class S = BinarySymbol, class Action = std::string>
class MatchSet : public ClassifierRepository<S, Action>
{
private:
    using ClassifierRepository<S, Action>::m_actionCount;
    
public:
    MatchSet(Population<S, Action> & population, const State<S> & situation)
    {
        for (auto && cl : population.classifiers())
        {
            if (cl.condition.contains(situation))
            {
                this->addClassifier(cl);
            }
        }

        /* TODO: the latter part of GENERATE MATCH SET */
    }
};