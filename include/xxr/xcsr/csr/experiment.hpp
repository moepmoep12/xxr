#pragma once

#include "../../xcs/experiment.hpp"
#include "../condition.hpp"
#include "symbol.hpp"
#include "../classifier.hpp"
#include "../constants.hpp"
#include "match_set.hpp"
#include "ga.hpp"
#include "../action_set.hpp"

namespace xxr { namespace xcsr_impl { namespace csr
{

    template <
        typename T,
        typename Action,
        class PredictionArray = xcs_impl::EpsilonGreedyPredictionArray<
            MatchSet<
                xcs_impl::Population<
                    xcs_impl::ClassifierPtrSet<
                        xcsr_impl::Classifier<
                            xcsr_impl::ConditionActionPair<xcsr_impl::Condition<Symbol<T>>, Action>,
                            xcsr_impl::Constants
                        >
                    >
                >
            >
        >,
        class ActionSet = xcsr_impl::ActionSet<
            GA<
                xcs_impl::Population<
                    xcs_impl::ClassifierPtrSet<
                        xcsr_impl::Classifier<
                            xcsr_impl::ConditionActionPair<xcsr_impl::Condition<Symbol<T>>, Action>,
                            xcsr_impl::Constants
                        >
                    >
                >
            >
        >
    >
    class Experiment : public xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>
    {
    public:
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::type;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::SymbolType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ConditionType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ActionType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ConditionActionPairType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ConstantsType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ClassifierType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ClassifierPtrSetType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::PopulationType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::MatchSetType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::PredictionArrayType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::GAType;
        using typename xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::ActionSetType;

        using xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::constants;

        // Constructor
        using xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::Experiment;

        virtual std::string dumpPopulation() const override
        {
            std::stringstream ss;

            ss  << "Condition[" << constants.minValue << "-" << constants.maxValue << "],"
                << "Condition[c;s],Action,prediction,epsilon,F,exp,ts,as,n,acc" << std::endl;

            for (auto && cl : this->m_population)
            {
                for (auto && symbol : cl->condition)
                {
                    ss << "|";

                    auto normalizedLowerLimit = (symbol.lower() - constants.minValue) / (constants.maxValue - constants.minValue);
                    auto normalizedUpperLimit = (symbol.upper() - constants.minValue) / (constants.maxValue - constants.minValue);

                    for (int i = 0; i < 10; ++i)
                    {
                        if (normalizedLowerLimit < i / 10.0 && (i + 1) / 10.0 < normalizedUpperLimit)
                        {
                            ss << "O";
                        }
                        else if ((i / 10.0 <= normalizedLowerLimit && normalizedLowerLimit <= (i + 1) / 10.0)
                            || (i / 10.0 <= normalizedUpperLimit && normalizedUpperLimit <= (i + 1) / 10.0))
                        {
                            ss << "o";
                        }
                        else
                        {
                            ss << ".";
                        }
                    }
                }
                ss << "|" << ",";

                ss  << cl->condition << ","
                    << cl->action << ","
                    << cl->prediction << ","
                    << cl->epsilon << ","
                    << cl->fitness << ","
                    << cl->experience << ","
                    << cl->timeStamp << ","
                    << cl->actionSetSize << ","
                    << cl->numerosity << ","
                    << cl->accuracy() << std::endl;
            }

            return ss.str();
        }

        virtual void switchToCondensationMode() noexcept override
        {
            constants.chi = 0.0;
            constants.mu = 0.0;
            constants.subsumptionTolerance = 0.0;
        }

        // Destructor
        virtual ~Experiment() = default;
    };

}}}
