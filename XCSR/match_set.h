#pragma once

#include "../XCS/match_set.h"

template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Population>
class MatchSet : public XCS::MatchSet<T, Action, Symbol, Condition, Classifier, Population>
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier>;
    using ClassifierPtrSet<Action, Classifier>::m_set;
    using ClassifierPtrSet<Action, Classifier>::m_constants;
    using ClassifierPtrSet<Action, Classifier>::m_actionChoices;

    // Destructor
    virtual ~MatchSet() = default;

    // GENERATE COVERING CLASSIFIER
    virtual ClassifierPtr generateCoveringClassifier(const std::vector<T> & situation, const std::unordered_set<Action> & unselectedActions, uint64_t timeStamp) const
    {
        std::vector<Symbol> symbols(situation.size());
        for (auto && symbol : cl->situation)
        {
            symbols.emplace_back(symbol, XCS::Random::nextDouble(0.0, m_constants.maxSpread));
        }

        return std::make_shared<Classifier>(symbols, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
    }
};