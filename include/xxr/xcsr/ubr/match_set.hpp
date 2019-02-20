#pragma once

#include "../../xcs/match_set.hpp"

namespace xxr { namespace xcsr_impl { namespace ubr
{

    template <typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet, class Population>
    class MatchSet : public xcs_impl::MatchSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using ClassifierPtrSet::m_set;
        using ClassifierPtrSet::m_constants;
        using ClassifierPtrSet::m_availableActions;

        // GENERATE COVERING CLASSIFIER
        virtual ClassifierPtr generateCoveringClassifier(const std::vector<typename Symbol::type> & situation, const std::unordered_set<Action> & unselectedActions, uint64_t timeStamp) const override
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

                double lower = Random::nextDouble(lowerMin, symbol);
                double upper = Random::nextDouble(symbol, upperMax);
                if (m_constants.doRangeRestriction)
                {
                    lower = std::max(lower, m_constants.minValue);
                    upper = std::min(upper, m_constants.maxValue);
                }

                if (Random::nextDouble() < 0.5)
                {
                    symbols.emplace_back(lower, upper);
                }
                else
                {
                    symbols.emplace_back(upper, lower);
                }
            }

            return std::make_shared<Classifier>(symbols, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
        }

    public:
        // Constructor
        using xcs_impl::MatchSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>::MatchSet;

        // Destructor
        virtual ~MatchSet() = default;
    };

}}}
