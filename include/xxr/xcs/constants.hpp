#pragma once

#include <cstdint>

namespace xxr { namespace xcs_impl
{

    struct Constants
    {
        // N
        //   The maximum size of the population
        //   (the sum of the classifier numerosities in micro-classifiers)
        //   Recommended: large enough
        uint64_t n = 400;

        // beta
        //   The learning rate for updating fitness, prediction, prediction error, and
        //   action set size estimate in XCS's classifiers
        //   Recommended: 0.1-0.2
        double beta = 0.2;

        // alpha
        //   The fall of rate in the fitness evaluation
        //   Recommended: 0.1
        double alpha = 0.1;

        // epsilon_0
        //   The error threshold under which the accuracy of a classifier is set to one
        //   Recommended: 1% of the maximum reward
        double epsilonZero = 10;

        // nu
        //   The exponent in the power function for the fitness evaluation
        //   Recommended: 5
        double nu = 5;

        // gamma
        //   The discount rate in multi-step problems
        //   Recommended: 0.71
        double gamma = 0.71;

        // theta_GA
        //   The threshold for the GA application in an action set
        //   Recommended: 25-50
        uint64_t thetaGA = 25;

        // chi
        //   The probability of applying crossover
        //   Recommended: 0.5-1.0
        double chi = 0.8;

        // crossoverMethod
        enum class CrossoverMethod
        {
            UNIFORM_CROSSOVER,
            ONE_POINT_CROSSOVER,
            TWO_POINT_CROSSOVER,
        };
        CrossoverMethod crossoverMethod = CrossoverMethod::TWO_POINT_CROSSOVER;

        // mu
        //   The probability of mutating one allele and the action
        //   Recommended: 0.01-0.05
        double mu = 0.04;

        // theta_del
        //   The experience threshold over which the fitness of a classifier may be
        //   considered in its deletion probability
        //   Recommended: 20
        uint64_t thetaDel = 20;

        // delta
        //   The fraction of the mean fitness of the population below which the fitness
        //   of a classifier may be considered in its vote for deletion
        //   Recommended: 0.1
        double delta = 0.1;

        // theta_sub
        //   The experience of a classifier required to be a subsumer
        //   Recommended: 20
        uint64_t thetaSub = 20;

        // tau
        //   The tournament size for selection [Butz et al., 2003]
        //   (set "0" to use the roulette-wheel selection)
        double tau = 0.0;

        // P_sharp
        //   The probability of using a don't care symbol in an allele when covering
        //   Recommended: 0.33
        double dontCareProbability = 0.33;

        // p_I
        //   The initial prediction value when generating a new classifier
        //   Recommended: very small (essentially zero)
        double initialPrediction = 0.01;

        // epsilon_I
        //   The initial prediction error value when generating a new classifier
        //   Recommended: very small (essentially zero)
        double initialEpsilon = 0.01;

        // F_I
        //   The initial fitness value when generating a new classifier
        //   Recommended: very small (essentially zero)
        double initialFitness = 0.01;

        // p_explr
        //   The probability during action selection of choosing the action uniform
        //   randomly
        //   Recommended: 0.5 (depends on the type of experiment)
        double exploreProbability = 1.0;

        // theta_mna
        //   The minimal number of actions that must be present in a match set [M],
        //   or else covering will occur
        //   Recommended: the number of available actions
        //                (or use "0" to set automatically)
        uint64_t thetaMna = 0;

        // doGASubsumption
        //   Whether offspring are to be tested for possible logical subsumption by
        //   parents
        bool doGASubsumption = true;

        // doActionSetSubsumption
        //   Whether action sets are to be tested for subsuming classifiers
        bool doActionSetSubsumption = true;

        // doActionMutation
        //   Whether to apply the mutation to the action
        bool doActionMutation = true;

        // useMAM
        //   Whether to use the moyenne adaptive modifee (MAM) for updating the
        //   prediction and the prediction error of classifiers
        bool useMAM = true;

        virtual ~Constants() = default;
    };

}}
