#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <unordered_set>
#include <cstdio>
#include <cstddef>
#include "../environment/environment.hpp"
#include "experiment_settings.hpp"
#include "simple_moving_average.hpp"

namespace xxr
{

    class ExperimentLogStream
    {
    private:
        std::ofstream m_ofs;

    protected:
        std::ostream & m_os;

    public:
        explicit ExperimentLogStream(const std::string & filename = "", bool useCoutWhenEmpty = true)
            : m_os(
                filename.empty()
                    ? (useCoutWhenEmpty ? std::cout : m_ofs)
                    : m_ofs)
        {
            m_ofs.open(filename);
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
        std::size_t m_count;

    public:
        explicit SMAExperimentLogStream(const std::string & filename = "", std::size_t smaWidth = 1, bool useCoutWhenEmpty = true)
            : ExperimentLogStream(filename, useCoutWhenEmpty)
            , m_sma(smaWidth)
        {
        }

        virtual void write(double value) override
        {
            if (m_os)
            {
                double smaValue = m_sma(value);
                if (++m_count >= m_sma.order())
                {
                    m_os << smaValue << std::flush;
                }
            }
        }

        virtual void writeLine(double value) override
        {
            if (m_os)
            {
                double smaValue = m_sma(value);
                if (++m_count >= m_sma.order())
                {
                    m_os << smaValue << std::endl;
                }
            }
        }
    };

    class AbstractExperimentHelper
    {
    public:
        AbstractExperimentHelper() = default;

        virtual ~AbstractExperimentHelper() = default;

        virtual void runIteration(std::size_t repeat = 1) = 0;

        virtual void switchToCondensationMode() = 0;
    };

    template <class Experiment, class Environment>
    class ExperimentHelper : public AbstractExperimentHelper
    {
    private:
        const ExperimentSettings m_settings;
        std::vector<std::unique_ptr<Experiment>> m_experiments;
        std::vector<std::unique_ptr<Environment>> m_explorationEnvironments;
        std::vector<std::unique_ptr<Environment>> m_exploitationEnvironments;
        std::function<void(Environment &)> m_explorationCallback;
        std::function<void(Environment &)> m_exploitationCallback;
        ExperimentLogStream m_classifierStream;
        SMAExperimentLogStream m_rewardLogStream;
        SMAExperimentLogStream m_stepCountLogStream;
        ExperimentLogStream m_populationSizeLogStream;
        double m_summaryRewardSum;
        double m_summaryPopulationSizeSum;
        double m_summaryStepCountSum;

        template <class... Args>
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
        template <class... Args>
        explicit ExperimentHelper(
            const ExperimentSettings & settings,
            Args && ... args,
            typename Experiment::ConstantsType constants,
            std::vector<std::unique_ptr<Environment>> && explorationEnvironments,
            std::vector<std::unique_ptr<Environment>> && exploitationEnvironments,
            std::function<void(Environment &)> explorationCallback = [](Environment &){},
            std::function<void(Environment &)> exploitationCallback = [](Environment &){}
        )
            : m_settings(settings)
            , m_experiments(
                makeExperiments(
                    settings,
                    std::forward<Args>(args)...,
                    dynamic_cast<AbstractEnvironment<typename Experiment::type, typename Experiment::ActionType> &>(*explorationEnvironments.at(0)).availableActions,
                    constants))
            , m_explorationEnvironments(std::move(explorationEnvironments))
            , m_exploitationEnvironments(std::move(exploitationEnvironments))
            , m_explorationCallback(std::move(explorationCallback))
            , m_exploitationCallback(std::move(exploitationCallback))
            , m_classifierStream(settings.outputFilenamePrefix + settings.outputClassifierFilename)
            , m_rewardLogStream(settings.outputFilenamePrefix + settings.outputRewardFilename, settings.smaWidth)
            , m_stepCountLogStream(settings.outputFilenamePrefix + settings.outputStepCountFilename, settings.smaWidth, false)
            , m_populationSizeLogStream(settings.outputFilenamePrefix + settings.outputPopulationSizeFilename, false)
            , m_summaryRewardSum(0.0)
            , m_summaryPopulationSizeSum(0.0)
            , m_summaryStepCountSum(0.0)
        {
            if (!settings.inputClassifierFilename.empty())
            {
                for (auto && experiment : m_experiments)
                {
                    experiment->loadPopulationCSV(settings.inputClassifierFilename, !settings.useInputClassifierToResume);
                }
            }
        }

        virtual ~ExperimentHelper() = default;

        virtual void runIteration(std::size_t repeat = 1) override
        {
            for (std::size_t i = 0; i < repeat; ++i)
            {
                // Exploitation
                if (m_settings.exploitationCount > 0)
                {
                    std::size_t totalStepCount = 0;
                    double rewardSum = 0;
                    double populationSizeSum = 0.0;
                    for (std::size_t j = 0; j < m_settings.seedCount; ++j)
                    {
                        for (std::size_t k = 0; k < m_settings.exploitationCount; ++k)
                        {
                            do
                            {
                                // Choose action
                                auto action = m_experiments[j]->exploit(m_exploitationEnvironments[j]->situation(), m_settings.updateInExploitation);

                                // Get reward
                                double reward = m_exploitationEnvironments[j]->executeAction(action);
                                m_summaryRewardSum += reward / m_settings.exploitationCount / m_settings.seedCount;
                                if (m_settings.updateInExploitation)
                                {
                                    m_experiments[j]->reward(reward, m_exploitationEnvironments[j]->isEndOfProblem());
                                }
                                rewardSum += reward;
                                populationSizeSum += m_experiments[j]->populationSize();

                                ++totalStepCount;

                                // Run callback if needed
                                m_exploitationCallback(*m_exploitationEnvironments[j]);
                            } while (!m_exploitationEnvironments[j]->isEndOfProblem());
                        }
                    }

                    if (m_settings.summaryInterval > 0 && (i + 1) % m_settings.summaryInterval == 0)
                    {
                        std::printf("%9u %11.3f %10.3f %8.3f\n",
                            static_cast<unsigned int>(i + 1),
                            m_summaryRewardSum / m_settings.summaryInterval,
                            m_summaryPopulationSizeSum / m_settings.summaryInterval,
                            m_summaryStepCountSum / m_settings.summaryInterval);
                        std::fflush(stdout);
                        m_summaryRewardSum = 0.0;
                        m_summaryPopulationSizeSum = 0.0;
                        m_summaryStepCountSum = 0.0;
                    }

                    m_rewardLogStream.writeLine(rewardSum / m_settings.exploitationCount / m_settings.seedCount);
                    m_stepCountLogStream.writeLine(static_cast<double>(totalStepCount) / m_settings.exploitationCount / m_settings.seedCount);
                    m_populationSizeLogStream.writeLine(populationSizeSum / m_settings.seedCount);
                }

                // Exploration
                for (std::size_t j = 0; j < m_settings.seedCount; ++j)
                {
                    for (std::size_t k = 0; k < m_settings.explorationCount; ++k)
                    {
                        do
                        {
                            // Get situation from environment
                            auto situation = m_explorationEnvironments[j]->situation();

                            // Choose action
                            auto action = m_experiments[j]->explore(situation);

                            // Get reward
                            double reward = m_explorationEnvironments[j]->executeAction(action);
                            m_experiments[j]->reward(reward, m_explorationEnvironments[j]->isEndOfProblem());

                            // Run callback if needed
                            m_explorationCallback(*m_explorationEnvironments[j]);
                        } while (!m_explorationEnvironments[j]->isEndOfProblem());
                    }
                }
            }
        }
        
        virtual void switchToCondensationMode() override
        {
            for (auto && experiment : m_experiments)
            {
                experiment->switchToCondensationMode();
            }
        }

        std::size_t seedCount() const noexcept
        {
            return m_settings.seedCount;
        }

        Experiment & experimentAt(std::size_t seedIdx)
        {
            return *m_experiments[seedIdx];
        }

        Environment & explorationEnvironmentAt(std::size_t seedIdx)
        {
            return *m_explorationEnvironments[seedIdx];
        }

        Environment & exploitationEnvironmentAt(std::size_t seedIdx)
        {
            return *m_exploitationEnvironments[seedIdx];
        }
    };

}
