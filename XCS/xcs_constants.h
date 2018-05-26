#pragma once

#include <cstdint>

struct XCSConstants
{
    // N
    //   The maximum size of the population
    //   (the sum of the classifier numerosities in micro-classifiers)
    //   Recommended: large enough
    uint64_t maxPopulationClassifierCount = 1000;

    // beta
    //   The learning rate for updating fitness, prediction, prediction error, and
    //   action set size estimate in XCS's classifiers
    //   Recommended: 0.1-0.2
    double learningRate = 0.1;

    // alpha
    //   The fall of rate in the fitness evaluation
    //   Recommended: 0.1
    double alpha = 0.1;

    // epsilon_0
    //   The error threshold under which the accuracy of a classifier is set to one
    //   Recommended: 1% of the maximum reward
    double predictionErrorThreshold = 1;

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
    double crossoverProbability = 0.8;

    // mu
    //   The probability of mutating one allele and the action
    //   Recommended: 0.01-0.05
    double mutationProbability = 0.04;

    // theta_del
    //   The experience threshold over which the fitness of a classifier may be
    //   considered in its deletion probability
    //   Recommended: 20
    double thetaDel = 20;

    // delta
    //   The fraction of the mean fitness of the population below which the fitness
    //   of a classifier may be considered in its vote for deletion
    //   Recommended: 0.1
    double delta = 0.1;

    // theta_sub
    //   The experience of a classifier required to be a subsumer
    //   Recommended: 20
    double thetaSub = 20;

    // P_sharp
    //   The probability of using a don't care symbol in an allele when covering
    //   Recommended: 0.33
    double generalizeProbability = 0.33;

    // p_I
    //   The initial prediction value when generating a new classifier
    //   Recommended: very small (essentially zero)
    double initialPrediction = 0.01;

    // epsilon_I
    //   The initial prediction error value when generating a new classifier
    //   Recommended: very small (essentially zero)
    double initialPredictionError = 0.01;

    // F_I
    //   The initial fitness value when generating a new classifier
    //   Recommended: very small (essentially zero)
    double initialFitness = 0.01;

    // p_explr
    //   The probability during action selection of choosing the action uniform
    //   randomly
    //   Recommended: 0.5 (depends on the type of experiment)
    double exploreProbability = 0.5;

    // doGASubsumption
    //   Whether offspring are to be tested for possible logical subsumption by
    //   parents
    bool doGASubsumption = true;

    // doActionSetSubsumption
    //   Whether action sets are to be tested for subsuming classifiers
    bool doActionSetSubsumption = true;
};