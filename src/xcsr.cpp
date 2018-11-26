#define __USE_MINGW_ANSI_STDIO 0
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <cstddef>

#include <cxxopts.hpp>

#include "common.h"
#include "XCSR_CS/experiment.h"
#include "XCSR_LU/experiment.h"
#include "XCSR_UB/experiment.h"
#include "environment/real_multiplexer_environment.h"
#include "environment/checkerboard_environment.h"
#include "environment/csv_environment.h"

using namespace XCSR;

int main(int argc, char *argv[])
{
    XCSR::Constants constants;

    // Parse command line arguments
    cxxopts::Options options(argv[0], "XCSR Classifier System");

    options
        .allow_unrecognised_options()
        .add_options()
        ("o,coutput", "Output the classifier csv filename", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("r,routput", "Output the reward log csv filename", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("n,noutput", "Output the macro-classifier count log csv filename", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        //("nsoutput", "Output the number of steps log csv filename in the multi-step problem", cxxopts::value<std::string>()->default_value(""), "FILENAME")
        ("m,mux", "Use the real multiplexer problem", cxxopts::value<int>(), "LENGTH")
        ("chk", "Use the n-dimentional checkerboard problem", cxxopts::value<int>(), "N")
        ("chk-div", "The division in the checkerboard problem", cxxopts::value<int>(), "DIVISION")
        ("c,csv", "Use the csv file", cxxopts::value<std::string>(), "FILENAME")
        ("e,csv-eval", "Use the csv file for evaluation", cxxopts::value<std::string>(), "FILENAME")
        ("csv-random", "Whether to choose lines in random order from the csv file", cxxopts::value<bool>()->default_value("true"), "true/false")
        ("i,iteration", "The number of iterations", cxxopts::value<uint64_t>()->default_value("20000"), "COUNT")
        ("avg-seeds", "The number of different random seeds for averaging the reward and the macro-classifier count", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("explore", "The exploration count for each iteration", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit", "The exploitation (= test mode) count for each iteration (set \"0\" if you don't need evaluation)", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("exploit-upd", "Whether to update classifier parameters in test mode (\"auto\": false for single-step & true for multi-step)", cxxopts::value<std::string>()->default_value("auto"), "auto/true/false")
        ("sma", "The width of the simple moving average for the reward log", cxxopts::value<uint64_t>()->default_value("1"), "COUNT")
        ("a,action", "The available action choices for csv (comma-separated, integer only)", cxxopts::value<std::string>(), "ACTIONS")
        ("repr", "The XCSR classifier representation (Center-Spread: \"cs\" / Lower-Upper [Ordered Bound]: \"lu\" / Unordered Bound: \"ub\")", cxxopts::value<std::string>()->default_value("cs"), "cs/lu/ub")
        ("N,max-population", "The maximum size of the population", cxxopts::value<uint64_t>()->default_value(std::to_string(constants.maxPopulationClassifierCount)), "COUNT")
        ("min-value", "The minimum value of each symbol in a situation", cxxopts::value<double>()->default_value(std::to_string(constants.minValue)), "VALUE")
        ("max-value", "The maximum value of each symbol in a situation", cxxopts::value<double>()->default_value(std::to_string(constants.maxValue)), "VALUE")
        ("alpha", "The fall of rate in the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(constants.alpha)), "ALPHA")
        ("beta", "The learning rate for updating fitness, prediction, prediction error, and action set size estimate in xcsr's classifiers", cxxopts::value<double>()->default_value(std::to_string(constants.learningRate)), "BETA")
        ("epsilon-0", "The error threshold under which the accuracy of a classifier is set to one", cxxopts::value<double>()->default_value(std::to_string(constants.alpha)), "EPSILON_0")
        ("nu", "The exponent in the power function for the fitness evaluation", cxxopts::value<double>()->default_value(std::to_string(constants.nu)), "NU")
        ("gamma", "The discount rate in multi-step problems", cxxopts::value<double>()->default_value(std::to_string(constants.gamma)), "GAMMA")
        ("theta-ga", "The threshold for the GA application in an action set", cxxopts::value<uint64_t>()->default_value(std::to_string(constants.thetaGA)), "THETA_GA")
        ("chi", "The probability of applying crossover", cxxopts::value<double>()->default_value(std::to_string(constants.crossoverProbability)), "CHI")
        ("mu", "The probability of mutating one allele and the action", cxxopts::value<double>()->default_value(std::to_string(constants.mutationProbability)), "MU")
        ("theta-del", "The experience threshold over which the fitness of a classifier may be considered in its deletion probability", cxxopts::value<double>()->default_value(std::to_string(constants.thetaDel)), "THETA_DEL")
        ("delta", "The fraction of the mean fitness of the population below which the fitness of a classifier may be considered in its vote for deletion", cxxopts::value<double>()->default_value(std::to_string(constants.delta)), "DELTA")
        ("theta-sub", "The experience of a classifier required to be a subsumer", cxxopts::value<double>()->default_value(std::to_string(constants.thetaSub)), "THETA_SUB")
        ("s-0", "The maximum value of a spread in the covering operator", cxxopts::value<double>()->default_value(std::to_string(constants.coveringMaxSpread)), "S_0")
        ("max-mutation", "The maximum change of a spread value or a center value in mutation", cxxopts::value<double>()->default_value(std::to_string(constants.mutationMaxChange)), "M")
        ("tol-sub", "The tolerance range of the upper and lower bound in the subsumption", cxxopts::value<double>()->default_value(std::to_string(constants.subsumptionTolerance)), "TOL_SUB")
        ("initial-prediction", "The initial prediction value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(constants.initialPrediction)), "P_I")
        ("initial-prediction-error", "The initial prediction error value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(constants.initialPredictionError)), "EPSILON_I")
        ("initial-fitness", "The initial fitness value when generating a new classifier", cxxopts::value<double>()->default_value(std::to_string(constants.initialFitness)), "F_I")
        ("p-explr", "The probability during action selection of choosing the action uniform randomly", cxxopts::value<double>()->default_value(std::to_string(constants.exploreProbability)), "P_EXPLR")
        ("theta-mna", "The minimal number of actions that must be present in a match set [M], or else covering will occur. Use \"0\" to set automatically.", cxxopts::value<uint64_t>()->default_value(std::to_string(constants.thetaMna)), "THETA_MNA")
        ("do-ga-subsumption", "Whether offspring are to be tested for possible logical subsumption by parents", cxxopts::value<bool>()->default_value(constants.doGASubsumption ? "true" : "false"), "true/false")
        ("do-action-set-subsumption", "Whether action sets are to be tested for subsuming classifiers", cxxopts::value<bool>()->default_value(constants.doActionSetSubsumption ? "true" : "false"), "true/false")
        ("do-action-mutation", "Whether to apply mutation to the action", cxxopts::value<bool>()->default_value(constants.doActionMutation ? "true" : "false"), "true/false")
        ("do-range-restriction", "Whether to restrict the range of the condition to the interval [min-value, max-value) in the covering and mutation operator (ignored when --repr=cs)", cxxopts::value<bool>()->default_value(constants.doRangeRestriction ? "true" : "false"), "true/false")
        ("do-covering-random-range-truncation", "Whether to truncate the covering random range before generating random intervals if the interval [x-s_0, x+s_0) is not contained in [min-value, max-value).  \"false\" is common for this option, but the covering operator can generate too many maximum-range intervals if s_0 is larger than (max-value - min-value) / 2.  Choose \"true\" to avoid the random bias in this situation.  (ignored when --repr=cs)", cxxopts::value<bool>()->default_value(constants.doCoveringRandomRangeTruncation ? "true" : "false"), "true/false")
        ("h,help", "Show this help");

    auto result = options.parse(argc, argv);

    // Set constants
    if (result.count("max-population"))
        constants.maxPopulationClassifierCount = result["max-population"].as<uint64_t>();
    if (result.count("min-value"))
        constants.minValue = result["min-value"].as<double>();
    if (result.count("max-value"))
        constants.maxValue = result["max-value"].as<double>();
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
    if (result.count("s-0"))
        constants.coveringMaxSpread = result["s-0"].as<double>();
    if (result.count("max-mutation"))
        constants.mutationMaxChange = result["max-mutation"].as<double>();
    if (result.count("tol-sub"))
        constants.subsumptionTolerance = result["tol-sub"].as<double>();
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
    if (result.count("do-action-mutation"))
        constants.doActionMutation = result["do-action-mutation"].as<bool>();
    if (result.count("do-range-restriction"))
        constants.doRangeRestriction = result["do-range-restriction"].as<bool>();
    if (result.count("do-covering-random-range-truncation"))
        constants.doCoveringRandomRangeTruncation = result["do-covering-random-range-truncation"].as<bool>();

    bool isEnvironmentSpecified = (result.count("mux") || result.count("csv") || result.count("chk"));

    // Determine whether to update classifier parameters in exploitation
    bool updateInExploitation;
    if (result["exploit-upd"].as<std::string>() == "true")
    {
        updateInExploitation = true;
    }
    else if (result["exploit-upd"].as<std::string>() == "false")
    {
        updateInExploitation = false;
    }
    else if (result["exploit-upd"].as<std::string>() == "auto")
    {
        updateInExploitation = false /* No multi-step problems for now */;
    }
    else
    {
        std::cerr << "Error: Unknown value for --exploit-upd (" << result["exploit-upd"].as<std::string>() << ")" << std::endl;
        exit(0);
    }

    // Show help
    if (result.count("help") || !isEnvironmentSpecified)
    {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }

    uint64_t iterationCount = result["iteration"].as<uint64_t>();
    uint64_t seedCount = result["avg-seeds"].as<uint64_t>();
    uint64_t explorationCount = result["explore"].as<uint64_t>();
    uint64_t exploitationCount = result["exploit"].as<uint64_t>();
    uint64_t smaWidth = result["sma"].as<uint64_t>();

    // Use multiplexer / checkerboard problem
    if (result.count("mux") || result.count("chk"))
    {
        std::vector<std::unique_ptr<XCS::AbstractEnvironment<double, bool>>> environments;
        if (result.count("mux"))
        {
            for (std::size_t i = 0; i < seedCount; ++i)
            {
                environments.push_back(std::make_unique<XCSR::RealMultiplexerEnvironment>(result["mux"].as<int>(), true));
            }
        }
        else
        {
            if (!result.count("chk-div"))
            {
                std::cerr << "Error: The division in the checkerboard problem (--chk-div) is not specified." << std::endl;
                exit(1);
            }
            for (std::size_t i = 0; i < seedCount; ++i)
            {
                environments.push_back(std::make_unique<XCSR::CheckerboardEnvironment>(result["chk"].as<int>(), result["chk-div"].as<int>()));
            }
        }

        if (result["repr"].as<std::string>() == "cs")
        {
            run<XCSR_CS::Experiment<double, bool>>(
                seedCount,
                constants,
                iterationCount,
                explorationCount,
                exploitationCount,
                updateInExploitation,
                result["coutput"].as<std::string>(),
                result["routput"].as<std::string>(),
                result["noutput"].as<std::string>(),
                "",//result["nsoutput"].as<std::string>(),
                smaWidth,
                environments,
                environments);
        }
        else if (result["repr"].as<std::string>() == "lu")
        {
            run<XCSR_LU::Experiment<double, bool>>(
                seedCount,
                constants,
                iterationCount,
                explorationCount,
                exploitationCount,
                updateInExploitation,
                result["coutput"].as<std::string>(),
                result["routput"].as<std::string>(),
                result["noutput"].as<std::string>(),
                "",//result["nsoutput"].as<std::string>(),
                smaWidth,
                environments,
                environments);
        }
        else if (result["repr"].as<std::string>() == "ub")
        {
            run<XCSR_UB::Experiment<double, bool>>(
                seedCount,
                constants,
                iterationCount,
                explorationCount,
                exploitationCount,
                updateInExploitation,
                result["coutput"].as<std::string>(),
                result["routput"].as<std::string>(),
                result["noutput"].as<std::string>(),
                "",//result["nsoutput"].as<std::string>(),
                smaWidth,
                environments,
                environments);
        }
        else
        {
            std::cerr << "Error: Unknown representation (" << result["repr"].as<std::string>() << ")" << std::endl;
            exit(1);
        }

        exit(0);
    }

    // Use csv file
    if (result.count("csv"))
    {
        // Get available action choices
        if (!result.count("action"))
        {
            std::cerr << "Error: Available action list (--action) is not specified." << std::endl;
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
                std::cerr << "Error: Action must be an integer." << std::endl;
                exit(1);
            }
        }

        std::string filename = result["csv"].as<std::string>();
        std::string evaluationCsvFilename = filename;
        if (result.count("csv-eval"))
        {
            evaluationCsvFilename = result["csv-eval"].as<std::string>();
        }

        std::vector<std::unique_ptr<XCS::AbstractEnvironment<double, int>>> explorationEnvironments;
        for (std::size_t i = 0; i < seedCount; ++i)
        {
            explorationEnvironments.push_back(std::make_unique<XCS::CSVEnvironment<double, int>>(filename, availableActions, result["csv-random"].as<bool>()));
        }
        std::vector<std::unique_ptr<XCS::AbstractEnvironment<double, int>>> exploitationEnvironments;
        for (std::size_t i = 0; i < seedCount; ++i)
        {
            exploitationEnvironments.push_back(std::make_unique<XCS::CSVEnvironment<double, int>>(evaluationCsvFilename, availableActions, result["csv-random"].as<bool>()));
        }

        if (result["repr"].as<std::string>() == "cs")
        {
            run<XCSR_CS::Experiment<double, int>>(
                seedCount,
                constants,
                iterationCount,
                explorationCount,
                exploitationCount,
                updateInExploitation,
                result["coutput"].as<std::string>(),
                result["routput"].as<std::string>(),
                result["noutput"].as<std::string>(),
                "",//result["nsoutput"].as<std::string>(),
                smaWidth,
                explorationEnvironments,
                exploitationEnvironments);
        }
        else if (result["repr"].as<std::string>() == "lu")
        {
            run<XCSR_LU::Experiment<double, int>>(
                seedCount,
                constants,
                iterationCount,
                explorationCount,
                exploitationCount,
                updateInExploitation,
                result["coutput"].as<std::string>(),
                result["routput"].as<std::string>(),
                result["noutput"].as<std::string>(),
                "",//result["nsoutput"].as<std::string>(),
                smaWidth,
                explorationEnvironments,
                exploitationEnvironments);
        }
        else if (result["repr"].as<std::string>() == "ub")
        {
            run<XCSR_UB::Experiment<double, int>>(
                seedCount,
                constants,
                iterationCount,
                explorationCount,
                exploitationCount,
                updateInExploitation,
                result["coutput"].as<std::string>(),
                result["routput"].as<std::string>(),
                result["noutput"].as<std::string>(),
                "",//result["nsoutput"].as<std::string>(),
                smaWidth,
                explorationEnvironments,
                exploitationEnvironments);
        }
        else
        {
            std::cerr << "Error: Unknown representation (" << result["repr"].as<std::string>() << ")" << std::endl;
            exit(1);
        }

        exit(0);
    }

    // No target environment (show help)
    std::cout << options.help({"", "Group"}) << std::endl;
    return 1;
}
