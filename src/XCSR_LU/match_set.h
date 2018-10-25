#pragma once

#include "../XCS/match_set.h"

namespace XCSR_LU
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
                double lowerMin = symbol - m_constants.coveringMaxSpread;
                double upperMax = symbol + m_constants.coveringMaxSpread;
                if (m_constants.doCoveringRandomRangeTruncation)
                {
                    lowerMin = std::max(lowerMin, m_constants.minValue);
                    upperMax = std::min(upperMax, m_constants.maxValue);
                }

                double lower = XCS::Random::nextDouble(lowerMin, symbol);
                double upper = XCS::Random::nextDouble(symbol, upperMax);
                if (m_constants.doRangeRestriction)
                {
                    lower = std::max(lower, m_constants.minValue);
                    upper = std::min(upper, m_constants.maxValue);
                }

                symbols.emplace_back(lower, upper);
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