#pragma once

#include "../XCS/experiment.h"
#include "symbol.h"
#include "match_set.h"
#include "constants.h"
#include "ga.h"
#include "environment.h"

namespace XCSR
{

    template <
        typename T,
        typename Action,
        class Symbol = Symbol<T>,
        class Condition = XCS::Condition<T, Symbol>,
        class ConditionActionPair = XCS::ConditionActionPair<T, Action, Symbol, Condition>,
        class Classifier = XCS::Classifier<T, Action, Symbol, Condition, ConditionActionPair>,
        class Constants = Constants,
        class ClassifierPtrSet = XCS::ClassifierPtrSet<Action, Classifier, Constants>,
        class Population = XCS::Population<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet>,
        class MatchSet = MatchSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population>,
        class PredictionArray = XCS::EpsilonGreedyPredictionArray<T, Action, Symbol, Condition, Classifier, MatchSet>,
        class GA = GA<T, Action, Symbol, Condition, Classifier, Population, Constants, ClassifierPtrSet>,
        class ActionSet = XCS::ActionSet<T, Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA>
    >
    class Experiment : public XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>
    {
    public:
        // Constructor
        using XCS::Experiment<T, Action, Symbol, Condition, ConditionActionPair, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, PredictionArray, GA, ActionSet>::Experiment;

        // Destructor
        virtual ~Experiment() = default;
    };

}