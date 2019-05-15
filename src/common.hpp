#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>
#include <type_traits>
#include <cassert>
#include <cstddef>

#include <xxr/xcsr.hpp>
#include <xxr/helper/csv.hpp>

#include "util/simple_moving_average.hpp"

template <class Experiment, class Constants, class Environment, class... Args>
std::unique_ptr<Experiment> run(
    std::size_t seedCount,
    const Constants & constants,
    std::size_t iterationCount,
    std::size_t condensationIterationCount,
    std::size_t explorationCount,
    std::size_t exploitationCount,
    bool updateInExploitation,
    const std::string & classifierLogFilename,
    const std::string & rewardLogFilename,
    const std::string & populationSizeLogFilename,
    const std::string & stepCountLogFilename,
    const std::string & initialPopulationFilename,
    bool useInitialPopulationForResume,
    std::size_t smaWidth,
    std::vector<std::unique_ptr<Environment>> & explorationEnvironments,
    std::vector<std::unique_ptr<Environment>> & exploitationEnvironments,
    std::function<void(Environment &)> explorationCallback = [](Environment &){},
    std::function<void(Environment &)> exploitationCallback = [](Environment &){},
    Args && ... args)
{
    assert(explorationEnvironments.size() == seedCount);
    assert(exploitationEnvironments.size() == seedCount);

    std::vector<std::unique_ptr<Experiment>> experiments;
    for (std::size_t i = 0; i < seedCount; ++i)
    {
        experiments.push_back(
            std::make_unique<Experiment>(
                std::forward<Args>(args)...,
                explorationEnvironments[i]->availableActions,
                constants
            )
        );
    }

    SimpleMovingAverage<double> sma(smaWidth);
    SimpleMovingAverage<double> stepCountSMA(smaWidth);

    std::ofstream classifierLogFileStream;
    std::ostream & classifierLogStream = classifierLogFilename.empty() ? std::cout
        : (classifierLogFileStream.open(classifierLogFilename), classifierLogFileStream);

    std::ofstream rewardLogFileStream;
    std::ostream & rewardLogStream = rewardLogFilename.empty() ? std::cout
        : (rewardLogFileStream.open(rewardLogFilename), rewardLogFileStream);

    bool outputsPopulationSizeLogFile = !populationSizeLogFilename.empty();
    std::ofstream populationSizeLogStream;
    if (outputsPopulationSizeLogFile)
    {
        populationSizeLogStream.open(populationSizeLogFilename);
    }

    bool outputsStepCountLogFile = !stepCountLogFilename.empty();
    std::ofstream stepCountLogStream;
    if (outputsStepCountLogFile)
    {
        stepCountLogStream.open(stepCountLogFilename);
    }

    // Load initial population
    if (!initialPopulationFilename.empty())
    {
        for (auto && experiment : experiments)
        {
            experiment->loadPopulationCSV(initialPopulationFilename, !useInitialPopulationForResume);
        }
    }

    for (std::size_t i = 0; i < iterationCount + condensationIterationCount; ++i)
    {
        // Switch to rule condensation mode after normal iterations
        if (i == iterationCount)
        {
            for (std::size_t j = 0; j < seedCount; ++j)
            {
                experiments[j]->switchToCondensationMode();
            }
        }

        // Exploitation
        if (exploitationCount > 0)
        {
            std::size_t totalStepCount = 0;
            double rewardSum = 0;
            for (std::size_t j = 0; j < seedCount; ++j)
            {
                for (std::size_t k = 0; k < exploitationCount; ++k)
                {
                    do
                    {
                        // Choose action
                        int action = experiments[j]->exploit(exploitationEnvironments[j]->situation(), updateInExploitation);

                        // Get reward
                        double reward = exploitationEnvironments[j]->executeAction(action);
                        if (updateInExploitation)
                        {
                            experiments[j]->reward(reward, exploitationEnvironments[j]->isEndOfProblem());
                        }
                        rewardSum += reward;

                        ++totalStepCount;

                        // Run callback if needed
                        exploitationCallback(*exploitationEnvironments[j]);
                    } while (!exploitationEnvironments[j]->isEndOfProblem());
                }
            }

            double rewardAverage = sma(rewardSum / exploitationCount / seedCount);
            double stepCountAverage = stepCountSMA(static_cast<double>(totalStepCount) / exploitationCount / seedCount);

            if (i >= smaWidth - 1)
            {
                rewardLogStream << rewardAverage << std::endl;

                if (outputsStepCountLogFile)
                {
                    stepCountLogStream << stepCountAverage << std::endl;
                }
            }

            if (outputsPopulationSizeLogFile)
            {
                std::size_t populationSizeSum = 0;
                for (std::size_t j = 0; j < seedCount; ++j)
                {
                    populationSizeSum += experiments[j]->populationSize();
                }
                populationSizeLogStream << (populationSizeSum / seedCount) << std::endl;
            }
        }

        // Exploration
        for (std::size_t j = 0; j < seedCount; ++j)
        {
            for (std::size_t k = 0; k < explorationCount; ++k)
            {
                do
                {
                    // Get situation from environment
                    auto situation = explorationEnvironments[j]->situation();

                    // Choose action
                    int action = experiments[j]->explore(situation);

                    // Get reward
                    double reward = explorationEnvironments[j]->executeAction(action);
                    experiments[j]->reward(reward, explorationEnvironments[j]->isEndOfProblem());

                    // Run callback if needed
                    explorationCallback(*explorationEnvironments[j]);
                } while (!explorationEnvironments[j]->isEndOfProblem());
            }
        }
    }

    if (classifierLogFilename.empty() && rewardLogFilename.empty())
    {
        std::cout << std::endl;
    }

    experiments[0]->dumpPopulation(classifierLogStream);
    classifierLogStream << std::endl;

    return std::move(experiments[0]);
}
