#include <iostream>
#include <memory>
#include <string>
#include <cstddef>

#include "condition.h"
#include "classifier.h"
#include "experiment.h"

using namespace XCS;

int main()
{
    std::size_t multiplexerLength = 11;

    Constants constants;

    if (multiplexerLength == 3)
    {
        constants.maxPopulationClassifierCount = 200;
    }
    else if (multiplexerLength == 6)
    {
        constants.maxPopulationClassifierCount = 400;
    }
    else if (multiplexerLength == 11)
    {
        constants.maxPopulationClassifierCount = 800;
    }
    else if (multiplexerLength == 20)
    {
        constants.maxPopulationClassifierCount = 2000;
        constants.generalizeProbability = 0.5;
    }
    else if (multiplexerLength == 37)
    {
        constants.maxPopulationClassifierCount = 5000;
        constants.generalizeProbability = 0.65;
    }
    else
    {
        constants.maxPopulationClassifierCount = 50000;
        constants.generalizeProbability = 0.75;
    }

    MultiplexerEnvironment environment(multiplexerLength);
    Experiment<bool, bool> xcs(environment.availableActions, constants);
    for (std::size_t i = 0; i < 500; ++i)
    {
        // Exploration
        for (std::size_t j = 0; j < 100; ++j)
        {
            // Choose action
            bool action = xcs.explore(environment.situation());

            // Get reward
            double reward = environment.executeAction(action);
            xcs.reward(reward);
        }

        // Exploitation
        double rewardSum = 0;
        for (std::size_t j = 0; j < 1000; ++j)
        {
            // Choose action
            bool action = xcs.exploit(environment.situation());

            // Get reward
            double reward = environment.executeAction(action);
            rewardSum += reward;
        }

        std::cout << (rewardSum / 1000) << std::endl;
    }
    std::cout << std::endl;

    xcs.dumpPopulation();

    return 0;
}