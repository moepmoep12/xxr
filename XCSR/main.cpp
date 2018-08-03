#include <iostream>
#include <memory>
#include <string>
#include <cstddef>

#include "experiment.h"

using namespace XCSR;

int main()
{
    std::size_t multiplexerLength = 6;

    Constants constants;

    constants.thetaGA = 12;

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
    }
    else if (multiplexerLength == 37)
    {
        constants.maxPopulationClassifierCount = 5000;
    }
    else
    {
        constants.maxPopulationClassifierCount = 50000;
    }

    RealMultiplexerEnvironment environment(multiplexerLength, true);
    Experiment<double, bool> xcsr(environment.availableActions, constants);
    for (std::size_t i = 0; i < 500; ++i)
    {
        // Exploration
        for (std::size_t j = 0; j < 100; ++j)
        {
            // Choose action
            bool action = xcsr.explore(environment.situation());

            // Get reward
            double reward = environment.executeAction(action);
            xcsr.reward(reward);
        }

        // Exploitation
        double rewardSum = 0;
        for (std::size_t j = 0; j < 1000; ++j)
        {
            // Choose action
            bool action = xcsr.exploit(environment.situation());

            // Get reward
            double reward = environment.executeAction(action);
            rewardSum += reward;
        }

        std::cout << (rewardSum / 1000) << std::endl;
    }
    std::cout << std::endl;

    xcsr.dumpPopulation();

    return 0;
}