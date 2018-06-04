#pragma once

#include "../XCS/experiment.h"
#include "symbol.h"
#include "constants.h"

namespace XCSR
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = XCS::Condition<T, Symbol>,
        class Classifier = XCS::Classifier<T, Action, Symbol, Condition>,
        class GA = XCS::GA<T, Action, Symbol, Condition, Classifier>,
        class Population = XCS::Population<T, Action, Symbol, Condition, Classifier>,
        class MatchSet = XCS::MatchSet<T, Action, Symbol, Condition, Classifier, Population>,
        class PredictionArray = XCS::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class ActionSet = XCS::ActionSet<T, Action, Symbol, Condition, Classifier, GA, Population, MatchSet>,
        class Constants = Constants
    >
    class Experiment : public XCS::Experiment<T, Action, Symbol, Condition, Classifier, GA, Population, MatchSet, PredictionArray, ActionSet, Constants>
    {
    public:
    };

}