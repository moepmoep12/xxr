#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <cstddef>

#include "environment/environment.h"
#include "util/simple_moving_average.h"

template <class Experiment, typename T, typename Action, class Constants, class Environment>
void run(
    std::size_t seedCount,
    const std::unordered_set<Action> & availableActions,
    const Constants & constants,
    std::size_t iterationCount,
    std::size_t explorationCount,
    std::size_t exploitationCount,
    const std::string & classifierLogFilename,
    const std::string & rewardLogFilename,
    const std::string & populationSizeLogFilename,
    std::size_t smaWidth,
    std::vector<std::unique_ptr<XCS::AbstractEnvironment<T, Action>>> & explorationEnvironments,
    std::vector<Environment> & exploitationEnvironments)
{
    assert(explorationEnvironments.size() == seedCount);
    assert(exploitationEnvironments.size() == seedCount);

    std::vector<Experiment> experiments;
    for (std::size_t i = 0; i < seedCount; ++i)
    {
        experiments.emplace_back(availableActions, constants);
    }

    SimpleMovingAverage<double> sma(smaWidth);

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

    for (std::size_t i = 0; i < iterationCount; ++i)
    {
        // Exploitation
        if (exploitationCount > 0)
        {
            double rewardSum = 0;
            for (std::size_t j = 0; j < seedCount; ++j)
            {
                for (std::size_t k = 0; k < exploitationCount; ++k)
                {
                    // Get situation from environment
                    auto situation = exploitationEnvironments[j]->situation();

                    // Choose action
                    int action = experiments[j].exploit(exploitationEnvironments[j]->situation());

                    // Get reward
                    double reward = exploitationEnvironments[j]->executeAction(action);
                    rewardSum += reward;
                }
            }

            double rewardAverage = sma(rewardSum / exploitationCount / seedCount);

            if (i >= smaWidth - 1)
            {
                rewardLogStream << rewardAverage << std::endl;
            }

            if (outputsPopulationSizeLogFile)
            {
                std::size_t populationSizeSum = 0;
                for (std::size_t j = 0; j < seedCount; ++j)
                {
                    populationSizeSum += experiments[j].populationSize();
                }
                populationSizeLogStream << (populationSizeSum / seedCount) << std::endl;
            }
        }

        // Exploration
        for (std::size_t j = 0; j < seedCount; ++j)
        {
            for (std::size_t k = 0; k < explorationCount; ++k)
            {
                // Get situation from environment
                auto situation = explorationEnvironments[j]->situation();

                // Choose action
                int action = experiments[j].explore(situation);

                // Get reward
                double reward = explorationEnvironments[j]->executeAction(action);
                experiments[j].reward(reward);
            }
        }
    }

    if (classifierLogFilename.empty() && rewardLogFilename.empty())
    {
        std::cout << std::endl;
    }

    classifierLogStream << experiments[0].dumpPopulation() << std::endl;
}
