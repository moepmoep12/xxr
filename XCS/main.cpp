#include <iostream>
#include <sstream>
#include <istream>
#include <memory>
#include <string>
#include <unordered_set>
#include <cstddef>

#include <cxxopts.hpp>

#include "condition.h"
#include "classifier.h"
#include "experiment.h"

using namespace XCS;

int main(int argc, char *argv[])
{
    // Parse command line arguments
    cxxopts::Options options(argv[0], "XCS Classifier System");
    options
        .allow_unrecognised_options()
        .add_options()
        ("m,mux", "Use multiplexer problem", cxxopts::value<int>(), "LENGTH")
        ("c,csv", "Use csv file", cxxopts::value<std::string>(), "FILENAME")
        ("r,repeat", "Repeat input count for csv", cxxopts::value<int>()->default_value("1"), "COUNT")
        ("a,action", "Available action choices for csv (comma-separated, integer only)", cxxopts::value<std::string>(), "ACTIONS")
        ("h,help", "Show help");

    auto result = options.parse(argc, argv);

    // Show help
    if (result.count("help") || (!result.count("mux") && !result.count("csv")))
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }

    // Use multiplexer problem
    if (result.count("mux"))
    {
        std::size_t multiplexerLength = result["mux"].as<int>();

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
        exit(0);
    }

    // Use csv file
    if (result.count("csv"))
    {
        Constants constants;

        std::string filename = result["csv"].as<std::string>();

        // Get available action choices
        if (!result.count("action"))
        {
            std::cout << "Error: Available action list (--action) is not specified." << std::endl;
            exit(1);
        }
        std::string availableActionsStr = result["action"].as<std::string>();
        std::string availableActionStr;
        std::stringstream ss(availableActionsStr);
        std::unordered_set<int> availableActions;
        while (std::getline(ss, availableActionStr, ','))
        {
            try
            {
                availableActions.insert(std::stoi(availableActionStr));
            }
            catch (std::exception & e)
            {
                std::cout << "Error: Action must be an integer." << std::endl;
                exit(1);
            }
        }

        Experiment<int, int> xcs(availableActions, constants);

        int repeatInputCount = result["repeat"].as<int>();
        for (int i = 0; i < repeatInputCount; ++i)
        {
            CSVEnvironment<int, int, Symbol<int>> environment(filename, availableActions);
            while (true)
            {
                // Get situation from environment
                auto situation = environment.situation();

                if (situation.size() == 0)
                {
                    break;
                }

                // Choose action
                bool action = xcs.explore(situation);

                // Get reward
                double reward = environment.executeAction(action);
                xcs.reward(reward);
            }
        }

        xcs.dumpPopulation();
        exit(0);
    }

    // No target environment (show help)
    std::cout << options.help({"", "Group"}) << std::endl;
    return 1;
}
