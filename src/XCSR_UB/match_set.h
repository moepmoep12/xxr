#pragma once

#include "../XCS/match_set.h"

namespace XCSR_UB
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet, class Population>
    class MatchSet : public XCS::MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using ClassifierPtrSet::m_set;
        using ClassifierPtrSet::m_constants;
        using ClassifierPtrSet::m_availableActions;

        // GENERATE COVERING CLASSIFIER
        virtual ClassifierPtr generateCoveringClassifier(const std::vector<T> & situation, const std::unordered_set<Action> & unselectedActions, uint64_t timeStamp) const override
        {
            std::vector<Symbol> symbols;
            for (auto && symbol : situation)
            {
                double l = symbol - XCS::Random::nextDouble(0.0, m_constants.maxSpread);
                double u = symbol + XCS::Random::nextDouble(0.0, m_constants.maxSpread);
                if (m_constants.doRangeRestriction)
                {
                    l = std::max(l, m_constants.minValue);
                    u = std::min(u, m_constants.maxValue);
                }

                if (XCS::Random::nextDouble() < 0.5)
                {
                    symbols.emplace_back(l, u);
                }
                else
                {
                    symbols.emplace_back(u, l);
                }
            }

            return std::make_shared<Classifier>(symbols, XCS::Random::chooseFrom(unselectedActions), timeStamp, m_constants);
        }

    public:
        // Constructor
        using XCS::MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>::MatchSet;

        // Destructor
        virtual ~MatchSet() = default;
    };

}