#pragma once

#include "../XCS/experiment.h"
#include "symbol.h"
#include "condition.h"
#include "match_set.h"
#include "constants.h"

namespace XCSR
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = Condition<T, Symbol>,
        class Classifier = XCS::Classifier<T, Action, Symbol, Condition>,
        class Constants = Constants,
        class ClassifierPtrSet = XCS::ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = XCS::Population<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = XCS::MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = XCS::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = XCS::GA<T, Action, Symbol, Condition, Classifier, Population, ClassifierPtrSet>,
        class ActionSet = XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment : public XCS::Experiment<T, Action, Symbol, Condition, Classifier, GA, Population, MatchSet, PredictionArray, ActionSet, Constants>
    {
    public:
        // Destructor
        virtual ~Experiment() = default;
    };

}