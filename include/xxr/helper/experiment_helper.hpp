#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>
#include <cstddef>

class ExperimentLogStream
{
private:
    std::ofstream m_ofs;
    std::ostream & m_os;
    SimpleMovingAverage<double> m_sma;

public:
    explicit ExperimentLogStream(const std::string & filename = "", bool useCoutWhenEmpty = true)
        : m_os(settings.outputRewardFilename.empty()
            ? (useCoutWhenEmpty ? std::cout : std::ostream())
            : (m_ofs.open(rewardLogFilename), m_ofs))
    {
    }

    virtual ~ExperimentLogStream() = default;

    void write(const std::string & str)
    {
        if (m_os)
        {
            m_os << str << std::flush;
        }
    }

    void writeLine(const std::string & str)
    {
        if (m_os)
        {
            m_os << str << std::endl;
        }
    }

    virtual void write(double value)
    {
        if (m_os)
        {
            m_os << value << std::flush;
        }
    }

    virtual void writeLine(double value)
    {
        if (m_os)
        {
            m_os << value << std::endl;
        }
    }
};

class SMAExperimentLogStream : public ExperimentLogStream
{
private:
    SimpleMovingAverage<double> m_sma;

public:
    explicit ExperimentLogStream(const std::string & filename = "", std::size_t smaWidth = 1, bool useCoutWhenEmpty = true)
        : ExperimentLogStream(filename, useCoutWhenEmpty)
        , m_sma(smaWidth)
    {
    }

    virtual void write(double value) override
    {
        if (m_os)
        {
            m_os << m_sma(value) << std::flush;
        }
    }

    virtual void writeLine(double value) override
    {
        if (m_os)
        {
            m_os << m_sma(value) << std::endl;
        }
    }
};

template <class Experiment>
class ExperimentHelper
{
private:
    const ExperimentSettings m_settings;
    std::vector<std::unique_ptr<Experiment>> m_experiments;
    ExperimentLogStream m_classifierStream;
    SMAExperimentLogStream m_rewardLogStream;
    SMAExperimentLogStream m_stepCountLogStream;
    ExperimentLogStream m_populationSizeLogStream;

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
        , m_classifierLogStream(settings.outputClassifierFilename)
        , m_rewardLogStream(settings.outputRewardFilename, settings.smaWidth)
        , m_stepCountLogStream(settings.outputStepCountFilename, settings.smaWidth)
        , m_populationSizeLogStream(settings.outputPopulationSizeFilename, false)
    {
        if (!settings.inputClassifierFilename.empty())
        {
            for (auto && experiment : m_experiments)
            {
                experiment->loadPopulationCSV(settings.inputClassifierFilename, !settings.useInputClassifierToResume);
            }
        }
    }

    void runIteration(std::size_t repeat = 1)
    {
        
    }
};
