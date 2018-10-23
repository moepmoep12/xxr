#pragma once

#include "../XCS/experiment.h"
#include "../XCS/condition.h"
#include "symbol.h"
#include "classifier.h"
#include "../XCSR/constants.h"
#include "match_set.h"
#include "ga.h"
#include "../XCSR/action_set.h"

namespace XCSR_LU
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = XCS::Condition<T, Symbol>,
        class ConditionActionPair = ConditionActionPair<T, Action, Symbol, Condition>,
        class Constants = XCSR::Constants,
        class Classifier = XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>,
        class ClassifierPtrSet = XCS::ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = XCS::Population<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = XCS::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>,
        class ActionSet = XCSR::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment : public XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>
    {
    protected:
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::m_constants;

    public:
        // Constructor
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::Experiment;

        virtual std::string dumpPopulation() const override
        {
            std::stringstream ss;

            ss  << "Condition[" << m_constants.minValue << "-" << m_constants.maxValue << "],"
                << "Condition[l;u],Action,prediction,epsilon,F,exp,ts,as,n" << std::endl;

            for (auto && cl : this->m_population)
            {
                for (auto && symbol : cl->condition)
                {
                    ss << "|";

                    auto normalizedLowerLimit = (symbol.lower - m_constants.minValue) / (m_constants.maxValue - m_constants.minValue);
                    auto normalizedUpperLimit = (symbol.upper - m_constants.minValue) / (m_constants.maxValue - m_constants.minValue);

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
                    << cl->predictionError << ","
                    << cl->fitness << ","
                    << cl->experience << ","
                    << cl->timeStamp << ","
                    << cl->actionSetSize << ","
                    << cl->numerosity << std::endl;
            }

            return ss.str();
        }

        // Destructor
        virtual ~Experiment() = default;
    };

}
