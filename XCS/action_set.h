#pragma once

#include <unordered_map>
#include <cstdint>

#include "classifier_ptr_set.h"
#include "match_set.h"

template <class Symbol, typename Action>
class ActionSet : public ClassifierPtrSet<Symbol, Action>
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;
    using ClassifierPtrSet<Symbol, Action>::m_set;

public:
    ActionSet() = default;

    ActionSet(const MatchSet<Symbol, Action> & matchSet, Action action)
    {
        for (auto && cl : matchSet)
        {
            if (cl->action == action)
            {
                m_set.insert(cl);
            }
        }
    }
};