#pragma once

#include "../../xcs/match_set.hpp"

namespace xxr { namespace xcsr_impl { namespace obr
{

    template <class Population>
    class MatchSet : public xcs_impl::MatchSet<Population>
    {
    public:
        using typename xcs_impl::MatchSet<Population>::type;
        using typename xcs_impl::MatchSet<Population>::SymbolType;
        using typename xcs_impl::MatchSet<Population>::ConditionType;
        using typename xcs_impl::MatchSet<Population>::ActionType;
        using typename xcs_impl::MatchSet<Population>::ConstantsType;
        using typename xcs_impl::MatchSet<Population>::ClassifierType;
        using typename xcs_impl::MatchSet<Population>::ClassifierPtrSetType;
        using typename xcs_impl::MatchSet<Population>::PopulationType;

    protected:
        using typename xcs_impl::MatchSet<Population>::ClassifierPtr;
        using xcs_impl::MatchSet<Population>::m_set;
        using xcs_impl::MatchSet<Population>::m_constants;
        using xcs_impl::MatchSet<Population>::m_availableActions;

        // GENERATE COVERING CLASSIFIER
        virtual ClassifierPtr generateCoveringClassifier(const std::vector<type> & situation, const std::unordered_set<ActionType> & unselectedActions, uint64_t timeStamp) const override
        {
            std::vector<SymbolType> symbols;
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

                symbols.emplace_back(lower, upper);
            }

            return std::make_shared<ClassifierType>(symbols, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
        }

    public:
        // Constructor
        using xcs_impl::MatchSet<Population>::MatchSet;

        // Destructor
        virtual ~MatchSet() = default;
    };

}}}
