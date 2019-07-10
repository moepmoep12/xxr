#pragma once

#include "../../xcs/match_set.hpp"

namespace xxr { namespace xcsr_impl { namespace csr
{

    template <class Population>
    class MatchSet : public xcs_impl::MatchSet<Population>
    {
    public:
        using type = typename xcs_impl::MatchSet<Population>::type;
        using SymbolType = typename xcs_impl::MatchSet<Population>::SymbolType;
        using ConditionType= typename xcs_impl::MatchSet<Population>::ConditionType;
        using ActionType=typename xcs_impl::MatchSet<Population>::ActionType;
        using ConditionActionPairType= typename xcs_impl::MatchSet<Population>::ConditionActionPairType;
        using ConstantsType= typename xcs_impl::MatchSet<Population>::ConstantsType;
        using ClassifierType=typename xcs_impl::MatchSet<Population>::ClassifierType;
        using StoredClassifierType=typename xcs_impl::MatchSet<Population>::StoredClassifierType;
        using ClassifierPtr=typename xcs_impl::MatchSet<Population>::ClassifierPtr;
        using ClassifierPtrSetType=typename xcs_impl::MatchSet<Population>::ClassifierPtrSetType;
        using PopulationType=typename xcs_impl::MatchSet<Population>::PopulationType;

    protected:
        using xcs_impl::MatchSet<Population>::m_set;
        using xcs_impl::MatchSet<Population>::m_constants;
        using xcs_impl::MatchSet<Population>::m_availableActions;

        // GENERATE COVERING CLASSIFIER
        virtual ClassifierPtr generateCoveringClassifier(const std::vector<type> & situation, const std::unordered_set<ActionType> & unselectedActions, uint64_t timeStamp) const override
        {
            std::vector<SymbolType> symbols;
            for (auto && symbol : situation)
            {
                symbols.emplace_back(symbol, Random::nextDouble(0.0, m_constants.coveringMaxSpread));
            }

            return std::make_shared<StoredClassifierType>(symbols, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
        }

    public:
        // Constructor
        MatchSet(ConstantsType & constants, const std::unordered_set<ActionType> & availableActions)
            : xcs_impl::MatchSet<Population>(constants, availableActions)
        {
        }

        MatchSet(Population & population, const std::vector<type> & situation, uint64_t timeStamp, ConstantsType & constants, const std::unordered_set<ActionType> & availableActions)
            : MatchSet(constants, availableActions)
        {
            this->regenerate(population, situation, timeStamp);
        }

        // Destructor
        virtual ~MatchSet() = default;
    };

}}}
