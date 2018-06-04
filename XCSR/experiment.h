#pragma once

#include "../XCS/experiment.h"
#include "symbol.h"
#include "constants.h"

namespace xcsr
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = xcs::Condition<T, Symbol>,
        class Classifier = xcs::Classifier<T, Action, Symbol, Condition>,
        class GA = xcs::GA<T, Action, Symbol, Condition, Classifier>,
        class Population = xcs::Population<T, Action, Symbol, Condition, Classifier>,
        class MatchSet = xcs::MatchSet<T, Action, Symbol, Condition, Classifier, Population>,
        class PredictionArray = xcs::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class ActionSet = xcs::ActionSet<T, Action, Symbol, Condition, Classifier, GA, Population, MatchSet>,
        class Constants = Constants
    >
    class Experiment : public xcs::Experiment<T, Action, Symbol, Condition, Classifier, GA, Population, MatchSet, PredictionArray, ActionSet, Constants>
    {
    public:
    };

}