#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <cstddef>

#include "repr.hpp"

namespace xxr { namespace xcsr_impl
{

    // XCSR experiment class
    // (not a base class for XCSR experiments)
    template <
        typename T,
        typename Action
    >
    class Experiment : public AbstractExperiment<T, Action>
    {
    protected:
        std::unique_ptr<AbstractExperiment<T, Action>> m_experiment;
        const Repr m_repr;

    public:
        // Constructor
        template <Repr R, class... Args>
        Experiment(Args && ... args)
            : m_experiment(std::make_unique<typename repr_to_experiment<R, T, Action>::type>(std::forward<Args>(args)...))
            , m_repr(R)
        {
        }

        template <class... Args>
        explicit Experiment(Repr repr, Args && ... args)
            : m_repr(repr)
        {
            switch(repr)
            {
            case Repr::CSR:
                m_experiment = std::make_unique<csr::Experiment<T, Action>>(std::forward<Args>(args)...);
                break;

            case Repr::OBR:
                m_experiment = std::make_unique<obr::Experiment<T, Action>>(std::forward<Args>(args)...);
                break;

            case Repr::UBR:
                m_experiment = std::make_unique<ubr::Experiment<T, Action>>(std::forward<Args>(args)...);
                break;

            default:
                assert(false);
            }
        }

        // Destructor
        virtual ~Experiment() = default;

        // Run with exploration
        virtual Action explore(const std::vector<T> & situation) override
        {
            return m_experiment->explore(situation);
        }

        virtual void reward(double value, bool isEndOfProblem = true) override
        {
            m_experiment->reward(value, isEndOfProblem);
        }

        // Run without exploration
        // (Set update to true when testing multi-step problems. If update is true, make sure to call reward() after this.)
        virtual Action exploit(const std::vector<T> & situation, bool update = false) override
        {
            return m_experiment->exploit(situation, update);
        }

        virtual void loadPopulationCSV(const std::string & filename, bool useAsInitialPopulation = true) override
        {
            m_experiment->loadPopulationCSV(filename, useAsInitialPopulation);
        }

        template <class Classifier>
        void setPopulation(const std::vector<Classifier> & classifiers, bool initTimeStamp = true)
        {
            m_experiment->setPopulation(classifiers, initTimeStamp);
        }

        virtual void dumpPopulation(std::ostream & os) const override
        {
            m_experiment->dumpPopulation(os);
        }

        virtual std::size_t populationSize() const noexcept override
        {
            return m_experiment->populationSize();
        }

        virtual void switchToCondensationMode() noexcept override
        {
            m_experiment->switchToCondensationMode();
        }

        virtual Repr repr() noexcept
        {
            return m_repr;
        }
    };

}}
