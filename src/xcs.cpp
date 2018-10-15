#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <cstddef>

#include <cxxopts.hpp>

#include "experiment.h"
#include "../environment/multiplexer_environment.h"
#include "../environment/csv_environment.h"

using namespace XCS;

int main(int argc, char *argv[])
{
    Constants constants;

    // Parse command line arguments
    cxxopts::Options options(argv[0], "XCS Classifier System");

    options
        .allow_unrecognised_options()
        .add_options()
        ("o,coutput", "Output the classifier csv filename", cxxopts::value<std::string>(), "FILENAME")
        ("r,routput", "Output the reward log csv filename", cxxopts::value<std::string>(), "FILENAME")
        ("n,noutput", "Output the macro-classifier count log csv filename", cxxopts::value<std::string>(), "FILENAME")
        ("m,mux", "Use the multiplexer problem", cxxopts::value<int>(), "LENGTH")
        ("c,csv", "Use the csv file", cxxopts::value<std::string>(), "FILENAME")
        ("e,csv-eval", "Use the csv file for evaluation", cxxopts::value<std::string>(), "FILENAME")
        ("csv-random", "Whether to choose lines in random order from the csv file", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("i,iteration", "The iteration count", cxxopts::value<uint64_t>()->default_value("100000"), "COUNT")
        ("explore", "The exploration count for each iteration", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit", "The exploitation count for each iteration (set \"0\" if you don't need evaluation)", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("a,action", "The available action choices for csv (comma-separated, integer only)", cxxopts::value<std::string>(), "ACTIONS")
        ("N,max-population", "The maximum size of the population", cxxopts::value<uint64_t>()->default_value(std::to_string(constants.maxPopulationClassifierCount)), "COUNT")
        ("alpha", "The fall of rate in the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(constants.alpha)), "ALPHA")
        ("beta", "The learning rate for updating fitness, prediction, prediction error, and action set size estimate in XCS's classifiers", cxxopts::value<double>()->default_value(std::to_string(constants.learningRate)), "BETA")
        ("epsilon-0", "The error threshold under which the accuracy of a classifier is set to one", cxxopts::value<double>()->default_value(std::to_string(constants.alpha)), "EPSILON_0")
        ("nu", "The exponent in the power function for the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(constants.nu)), "NU")
        ("gamma", "The discount rate in multi-step problems", cxxopts::value<double>()->default_value(std::to_string(constants.gamma)), "GAMMA")
        ("theta-ga", "The threshold for the GA application in an action set", cxxopts::value<uint64_t>()->default_value(std::to_string(constants.thetaGA)), "THETA_GA")
        ("chi", "The probability of applying crossover", cxxopts::value<double>()->default_value(std::to_string(constants.crossoverProbability)), "CHI")
        ("mu", "The probability of mutating one allele and the action", cxxopts::value<double>()->default_value(std::to_string(constants.mutationProbability)), "MU")
        ("theta-del", "The experience threshold over which the fitness of a classifier may be considered in its deletion probability", cxxopts::value<double>()->default_value(std::to_string(constants.thetaDel)), "THETA_DEL")
        ("delta", "The fraction of the mean fitness of the population below which the fitness of a classifier may be considered in its vote for deletion", cxxopts::value<double>()->default_value(std::to_string(constants.delta)), "DELTA")
        ("theta-sub", "The experience of a classifier required to be a subsumer", cxxopts::value<double>()->default_value(std::to_string(constants.thetaSub)), "THETA_SUB")
        ("s,p-sharp", "The probability of using a don't care symbol in an allele when covering", cxxopts::value<double>()->default_value(std::to_string(constants.generalizeProbability)), "P_SHARP")
        ("initial-prediction", "The initial prediction value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(constants.initialPrediction)), "P_I")
        ("initial-prediction-error", "The initial prediction error value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(constants.initialPredictionError)), "EPSILON_I")
        ("initial-fitness", "The initial fitness value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(constants.initialFitness)), "F_I")
        ("p-explr", "The probability during action selection of choosing the action uniform randomly", cxxopts::value<double>()->default_value(std::to_string(constants.exploreProbability)), "P_EXPLR")
        ("theta-mna", "The minimal number of actions that must be present in a match set [M], or else covering will occur. Use \"0\" to set automatically.", cxxopts::value<uint64_t>()->default_value(std::to_string(constants.thetaMna)), "THETA_MNA")
        ("do-ga-subsumption", "Whether offspring are to be tested for possible logical subsumption by parents", cxxopts::value<bool>()->default_value(constants.doGASubsumption ? "true" : "false"), "true/false")
        ("do-action-set-subsumption", "Whether action sets are to be tested for subsuming classifiers", cxxopts::value<bool>()->default_value(constants.doActionSetSubsumption ? "true" : "false"), "true/false")
        ("h,help", "Show this help");

    auto result = options.parse(argc, argv);

    // Set constants
    if (result.count("max-population"))
        constants.maxPopulationClassifierCount = result["max-population"].as<uint64_t>();
    if (result.count("alpha"))
        constants.alpha = result["alpha"].as<double>();
    if (result.count("beta"))
        constants.learningRate = result["beta"].as<double>();
    if (result.count("epsilon-0"))
        constants.predictionErrorThreshold = result["epsilon-0"].as<double>();
    if (result.count("nu"))
        constants.nu = result["nu"].as<double>();
    if (result.count("gamma"))
        constants.gamma = result["gamma"].as<double>();
    if (result.count("theta-ga"))
        constants.thetaGA = result["theta-ga"].as<uint64_t>();
    if (result.count("chi"))
        constants.crossoverProbability = result["chi"].as<double>();
    if (result.count("mu"))
        constants.mutationProbability = result["mu"].as<double>();
    if (result.count("theta-del"))
        constants.thetaDel = result["theta-del"].as<double>();
    if (result.count("delta"))
        constants.delta = result["delta"].as<double>();
    if (result.count("theta-sub"))
        constants.thetaSub = result["theta-sub"].as<double>();
    if (result.count("p-sharp"))
        constants.generalizeProbability = result["p-sharp"].as<double>();
    if (result.count("initial-prediction"))
        constants.initialPrediction = result["initial-prediction"].as<double>();
    if (result.count("initial-prediction-error"))
        constants.initialPredictionError = result["initial-prediction-error"].as<double>();
    if (result.count("initial-fitness"))
        constants.initialFitness = result["initial-fitness"].as<double>();
    if (result.count("p-explr"))
        constants.exploreProbability = result["p-explr"].as<double>();
    if (result.count("theta-mna"))
        constants.thetaMna = result["theta-mna"].as<uint64_t>();
    if (result.count("do-ga-subsumption"))
        constants.doGASubsumption = result["do-ga-subsumption"].as<bool>();
    if (result.count("do-action-set-subsumption"))
        constants.doActionSetSubsumption = result["do-action-set-subsumption"].as<bool>();
    
    bool isEnvironmentSpecified = (result.count("mux") || result.count("csv"));

    // Show help
    if (result.count("help") || !isEnvironmentSpecified)
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }

    uint64_t iterationCount = result["iteration"].as<uint64_t>();
    uint64_t explorationCount = result["explore"].as<uint64_t>();
    uint64_t exploitationCount = result["exploit"].as<uint64_t>();

    bool outputsRewardLogFile = result.count("routput");
    std::ofstream rewardLogStream;
    if (outputsRewardLogFile)
    {
        rewardLogStream = std::ofstream(result["routput"].as<std::string>());
    }

    bool outputsPopulationSizeLogFile = result.count("noutput");
    std::ofstream populationSizeLogStream;
    if (result.count("noutput"))
    {
        populationSizeLogStream = std::ofstream(result["noutput"].as<std::string>());
    }

    // Use multiplexer problem
    if (result.count("mux"))
    {
        std::size_t multiplexerLength = result["mux"].as<int>();

        MultiplexerEnvironment environment(multiplexerLength);
        Experiment<bool, bool> xcs(environment.availableActions, constants);
        for (std::size_t i = 0; i < iterationCount; ++i)
        {
            // Exploration
            for (std::size_t j = 0; j < explorationCount; ++j)
            {
                // Choose action
                bool action = xcs.explore(environment.situation());

                // Get reward
                double reward = environment.executeAction(action);
                xcs.reward(reward);
            }

            // Exploitation
            if (exploitationCount > 0)
            {
                double rewardSum = 0;
                for (std::size_t j = 0; j < exploitationCount; ++j)
                {
                    // Choose action
                    bool action = xcs.exploit(environment.situation());

                    // Get reward
                    double reward = environment.executeAction(action);
                    rewardSum += reward;
                }

                if (outputsRewardLogFile)
                {
                    rewardLogStream << (rewardSum / exploitationCount) << std::endl;
                }
                else
                {
                    std::cout << (rewardSum / exploitationCount) << std::endl;
                }

                if (outputsPopulationSizeLogFile)
                {
                    populationSizeLogStream << xcs.populationSize() << std::endl;
                }
            }
        }

        if (!outputsRewardLogFile)
        {
            std::cout << std::endl;
        }

        if (result.count("coutput"))
        {
            std::ofstream ofs(result["coutput"].as<std::string>());
            ofs << xcs.dumpPopulation() << std::endl;
        }
        else
        {
            std::cout << xcs.dumpPopulation() << std::endl;
        }

        exit(0);
    }

    // Use csv file
    if (result.count("csv"))
    {

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

        std::string filename = result["csv"].as<std::string>();
        std::string evaluationCsvFilename = filename;
        if (result.count("csv-eval"))
        {
            evaluationCsvFilename = result["csv-eval"].as<std::string>();
        }

        Experiment<int, int> xcs(availableActions, constants);
        CSVEnvironment<int, int> environment(filename, availableActions, result.count("csv-random"));
        CSVEnvironment<int, int> evaluationEnvironment(evaluationCsvFilename, availableActions, result.count("csv-random"));

        for (std::size_t i = 0; i < iterationCount; ++i)
        {
            // Exploitation
            if (exploitationCount > 0)
            {
                double rewardSum = 0;
                for (std::size_t j = 0; j < exploitationCount; ++j)
                {
                    // Get situation from environment
                    auto situation = evaluationEnvironment.situation();

                    // Choose action
                    int action = xcs.exploit(evaluationEnvironment.situation());

                    // Get reward
                    double reward = evaluationEnvironment.executeAction(action);
                    rewardSum += reward;
                }

                if (outputsRewardLogFile)
                {
                    rewardLogStream << (rewardSum / exploitationCount) << std::endl;
                }
                else
                {
                    std::cout << (rewardSum / exploitationCount) << std::endl;
                }

                if (outputsPopulationSizeLogFile)
                {
                    populationSizeLogStream << xcs.populationSize() << std::endl;
                }
            }

            // Exploration
            for (std::size_t j = 0; j < explorationCount; ++j)
            {
                // Get situation from environment
                auto situation = environment.situation();

                // Choose action
                int action = xcs.explore(situation);

                // Get reward
                double reward = environment.executeAction(action);
                xcs.reward(reward);
            }
        }

        if (!outputsRewardLogFile)
        {
            std::cout << std::endl;
        }

        if (result.count("coutput"))
        {
            std::ofstream ofs(result["coutput"].as<std::string>());
            ofs << xcs.dumpPopulation() << std::endl;
        }
        else
        {
            std::cout << xcs.dumpPopulation() << std::endl;
        }

        exit(0);
    }

    // No target environment (show help)
    std::cout << options.help({"", "Group"}) << std::endl;
    return 1;
}
