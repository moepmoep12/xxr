#pragma once

#include "../../xcs/experiment.hpp"
#include "../condition.hpp"
#include "symbol.hpp"
#include "../classifier.hpp"
#include "../constants.hpp"
#include "match_set.hpp"
#include "ga.hpp"
#include "../action_set.hpp"

namespace xxr { namespace xcsr_impl { namespace obr
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
        using xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::constants;

        // Constructor
        using xcs_impl::Experiment<T, Action, PredictionArray, ActionSet>::Experiment;

        virtual std::string dumpPopulation() const override
        {
            std::stringstream ss;

            ss  << "Condition[" << constants.minValue << "-" << constants.maxValue << "],"
                << "Condition[l;u],Action,prediction,epsilon,F,exp,ts,as,n,acc" << std::endl;

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
