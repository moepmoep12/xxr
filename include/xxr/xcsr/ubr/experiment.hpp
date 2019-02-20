#pragma once

#include "../../xcs/experiment.hpp"
#include "../condition.hpp"
#include "symbol.hpp"
#include "../classifier.hpp"
#include "../constants.hpp"
#include "match_set.hpp"
#include "ga.hpp"
#include "../action_set.hpp"

namespace xxr { namespace xcsr_impl { namespace ubr
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = xcsr_impl::Condition<Symbol>,
        class ConditionActionPair = xcsr_impl::ConditionActionPair<Action, Symbol, Condition>,
        class Constants = xcsr_impl::Constants,
        class Classifier = xcsr_impl::Classifier<Action, Symbol, Condition, ConditionActionPair, Constants>,
        class ClassifierPtrSet = xcs_impl::ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = xcs_impl::Population<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = MatchSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = xcs_impl::EpsilonGreedyPredictionArray<Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = GA<Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>,
        class ActionSet = xcsr_impl::ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment : public xcs_impl::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>
    {
    public:
        using xcs_impl::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::constants;

        // Constructor
        using xcs_impl::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::Experiment;

        virtual std::string dumpPopulation() const override
        {
            std::stringstream ss;

            ss  << "Condition[" << constants.minValue << "-" << constants.maxValue << "],"
                << "Condition[p;q],Action,prediction,epsilon,F,exp,ts,as,n,acc" << std::endl;

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
