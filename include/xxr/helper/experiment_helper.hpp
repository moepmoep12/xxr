#include <memory>
#include <unordered_set>

template <class Experiment>
class ExperimentHelper
{
private:
    const ExperimentSettings m_settings;
    std::vector<std::unique_ptr<Experiment>> m_experiments;
    SimpleMovingAverage<double> m_rewardSMA;
    SimpleMovingAverage<double> m_stepCountSMA;
    classifierLogFileStream;

    std::vector<std::unique_ptr<Experiment>> makeExperiments(
        const ExperimentSettings & settings,
        Args && ... args,
        const std::unordered_set<typename Experiment::ActionType> & availableActions,
        typename Experiment::ConstantsType constants)
    {
        std::vector<std::unique_ptr<Experiment>> experiments;
        for (std::size_t i = 0; i < settings.seedCount; ++i)
        {
            experiments.push_back(
                std::make_unique<Experiment>(
                    std::forward<Args>(args)...,
                    availableActions,
                    constants
                )
            );
        }
        return std::move(experiments);
    }

public:
    template <Args && ... args>
    explicit ExperimentHelper(
        const ExperimentSettings & settings,
        Args && ... args,
        const std::unordered_set<typename Experiment::ActionType> & availableActions,
        typename Experiment::ConstantsType constants
    )
        : m_settings(settings)
        , m_experiments(
            makeExperiments(std::forward<Args>(args)..., availableActions, constants))
        , m_rewardSMA(settings.smaWidth)
        , m_stepCountSMA(settings.smaWidth)
    {}

    void runIteration(std::size_t repeat = 1)
    {
        
    }
}
