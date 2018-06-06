#pragma once

#include "../XCS/experiment.h"
#include "symbol.h"
#include "classifier.h"
#include "constants.h"
#include "match_set.h"
#include "ga.h"
#include "action_set.h"
#include "environment.h"

namespace XCSR
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = XCS::Condition<T, Symbol>,
        class ConditionActionPair = ConditionActionPair<T, Action, Symbol, Condition>,
        class Classifier = XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair>,
        class Constants = Constants,
        class ClassifierPtrSet = XCS::ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = XCS::Population<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = XCS::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>,
        class ActionSet = ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment : public XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>
    {
    private:
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::m_constants;

    public:
        // Constructor
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::Experiment;

        virtual void dumpPopulation(double dontCareSpread = 0.03) const
        {
            std::cout << "C[c(s)]:A,C[c/#]:A,prediction,epsilon,F,exp,ts,as,n" << std::endl;
            for (auto && cl : this->m_population)
            {
                std::cout << *cl << ",";

                for (auto && symbol : cl->condition)
                {
                    if (symbol.center - symbol.spread <= m_constants.minValue + dontCareSpread && m_constants.maxValue - dontCareSpread <= symbol.center + symbol.spread)
                    {
                        std::cout << "# ";
                    }
                    else
                    {
                        std::cout << symbol.center << " ";
                    }
                }

                std::cout
                    << ":" << cl->action << ","
                    << cl->prediction << ","
                    << cl->predictionError << ","
                    << cl->fitness << ","
                    << cl->experience << ","
                    << cl->timeStamp << ","
                    << cl->actionSetSize << ","
                    << cl->numerosity << std::endl;
            }
        }

        // Destructor
        virtual ~Experiment() = default;
    };

}