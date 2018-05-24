#pragma once

#include <cstdint>

struct XCSConstants
{
    // The maximum size of the population (N)
    // (the sum of the classifier numerosities in micro-classifiers)
    // Recommended: large enough
    uint32_t maxPopulationClassifierCount = 1000;

    // The learning rate for updating fitness, prediction, prediction error, and
    // action set size estimate in XCS's classifiers (beta)
    // Recommended: 0.1-0.2
    double learningRate = 0.1;

    // The fall of rate in the fitness evaluation (alpha)
    // Recommended: 0.1
    double alpha = 0.1;

    // The error threshold under which the accuracy of a classifier is set to one
    // (epsilon_0)
    // Recommended: 1% of the maximum reward
    double predictionErrorThreshold = 1;

    // The exponent in the power function for the fitness evaluation (nu)
    // Recommended: 5
    double nu = 5;

    // The discount rate in multi-step problems (gamma)
    // Recommended: 0.71
    double gamma = 0.71;

    // The threshold for the GA application in an action set (theta_GA)
    // Recommended: 25-50
    uint32_t thetaGA = 25;

    // The probability of applying crossover (chi)
    // Recommended: 0.5-1.0
    double crossoverProbability = 0.8;

    // The probability of mutating one allele and the action (mu)
    // Recommended: 0.01-0.05
    double mutationProbability = 0.04;

    // The threshold over which the fitness of a classifier may be considered in
    // its deletion probability (theta_del)
    // Recommended: 20
    double thetaDel = 20;

    // The fraction of the mean fitness of the population below which the fitness
    // of a classifier may be considered in its vote for deletion (delta)
    // Recommended: 0.1
    double delta = 0.1;

    // The experience of a classifier required to be a subsumer (theta_sub)
    // Recommended: 20
    double thetaSub = 20;

    // The probability of using a don't care symbol in an allele when covering
    // (P_sharp)
    // Recommended: 0.33
    double generalizeProbability = 0.33;

    // The initial prediction value when generating a new classifier (p_I)
    // Recommended: very small (essentially zero)
    double initialPrediction = 0.01;

    // The initial prediction error value when generating a new classifier
    // (epsilon_I)
    // Recommended: very small (essentially zero)
    double initialPredictionError = 0.01;

    // The initial fitness value when generating a new classifier (F_I)
    // Recommended: very small (essentially zero)
    double initialFitness = 0.01;

    // The probability during action selection of choosing the action uniform
    // randomly (p_explr)
    // Recommended: 0.5 (depends on the type of experiment)
    double exploreProbability = 0.5;

    // Whether offspring are to be tested for possible logical subsumption by
    // parents
    bool doGASubsumption = true;

    // Whether action sets are to be tested for subsuming classifiers
    bool doActionSetSubsumption = true;
};