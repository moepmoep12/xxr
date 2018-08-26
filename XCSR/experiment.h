#pragma once

#include "../XCS/experiment.h"
#include "../XCS/condition.h"
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
        class Constants = Constants,
        class Classifier = XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair, Constants>,
        class ClassifierPtrSet = XCS::ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = XCS::Population<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = XCS::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>,
        class ActionSet = ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment : public XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>
    {
    private:
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::m_constants;

    public:
        // Constructor
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Constants, Classifier, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::Experiment;

        virtual void dumpPopulation() const
        {
            std::cout
                << "Condition[" << m_constants.minValue << "-" << m_constants.maxValue << "],"
                << "Condition[c(s)],Action,prediction,epsilon,F,exp,ts,as,n" << std::endl;

            for (auto && cl : this->m_population)
            {
                for (auto && symbol : cl->condition)
                {
                    std::cout << "|";

                    auto normalizedLowerLimit = (symbol.center - symbol.spread - m_constants.minValue) / (m_constants.maxValue - m_constants.minValue);
                    auto normalizedUpperLimit = (symbol.center + symbol.spread - m_constants.minValue) / (m_constants.maxValue - m_constants.minValue);

                    for (int i = 0; i < 10; ++i)
                    {
                        if (normalizedLowerLimit < i / 10.0 && (i + 1) / 10.0 < normalizedUpperLimit)
                        {
                            std::cout << "O";
                        }
                        else if ((i / 10.0 <= normalizedLowerLimit && normalizedLowerLimit <= (i + 1) / 10.0)
                            || (i / 10.0 <= normalizedUpperLimit && normalizedUpperLimit <= (i + 1) / 10.0))
                        {
                            std::cout << "o";
                        }
                        else
                        {
                            std::cout << ".";
                        }
                    }
                }
                std::cout << "|" << ",";

                std::cout
                    << cl->condition << ","
                    << cl->action << ","
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
