#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <cstddef>

namespace xxr { namespace xcs_impl
{

    template <typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet, class Population, class MatchSet, class GA>
    class ActionSet : public ClassifierPtrSet
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using ClassifierPtrSet::m_set;
        using ClassifierPtrSet::m_constants;
        using ClassifierPtrSet::m_availableActions;

        GA m_ga;

        // UPDATE FITNESS
        virtual void updateFitness()
        {
            double accuracySum = 0.0;

            for (auto && cl : m_set)
            {
                accuracySum += cl->accuracy() * cl->numerosity;
            }

            for (auto && cl : m_set)
            {
                cl->fitness += m_constants.beta * (cl->accuracy() * cl->numerosity / accuracySum - cl->fitness);
            }
        }

        // DO ACTION SET SUBSUMPTION
        virtual void doSubsumption(Population & population)
        {
            ClassifierPtr cl;

            for (auto && c : m_set)
            {
                if (c->isSubsumer())
                {
                    std::size_t cDontCareCount;
                    std::size_t clDontCareCount;
                    if ((cl.get() == nullptr) ||
                        ((cDontCareCount = c->condition.dontCareCount()) > (clDontCareCount = cl->condition.dontCareCount())) ||
                        ((cDontCareCount == clDontCareCount) && (Random::nextDouble() < 0.5)))
                    {
                        cl = c;
                    }
                }
            }

            if (cl.get() != nullptr)
            {
                std::vector<const ClassifierPtr *> removedClassifiers;
                for (auto && c : m_set)
                {
                    if (cl->isMoreGeneral(*c))
                    {
                        cl->numerosity += c->numerosity;
                        removedClassifiers.push_back(&c);
                    }
                }

                for (auto && removedClassifier : removedClassifiers)
                {
                    population.erase(*removedClassifier);
                    m_set.erase(*removedClassifier);
                }
            }
        }

    public:
        // Constructor
        ActionSet(Constants & constants, const std::unordered_set<Action> & availableActions) :
            ClassifierPtrSet(constants, availableActions),
            m_ga(constants, availableActions)
        {
        }

        ActionSet(const MatchSet & matchSet, Action action, Constants & constants, const std::unordered_set<Action> & availableActions) :
            ClassifierPtrSet(constants, availableActions),
            m_ga(constants, availableActions)
        {
            regenerate(matchSet, action);
        }

        // Destructor
        virtual ~ActionSet() = default;

        // GENERATE ACTION SET
        virtual void regenerate(const MatchSet & matchSet, Action action)
        {
            m_set.clear();

            for (auto && cl : matchSet)
            {
                if (cl->action == action)
                {
                    m_set.insert(cl);
                }
            }
        }

        virtual void copyTo(ActionSet<Action, Symbol, Condition, Classifier, Constants, ClassifierPtrSet, Population, MatchSet, GA> & dest)
        {
            dest.m_set = m_set; // don't copy m_ga since it contains const parameters
        }

        // RUN GA (refer to GA::run() for the latter part)
        virtual void runGA(const std::vector<typename Symbol::type> & situation, Population & population, uint64_t timeStamp)
        {
            uint64_t timeStampNumerositySum = 0;
            uint64_t numerositySum = 0;

            for (auto && cl : m_set)
            {
                timeStampNumerositySum += cl->timeStamp * cl->numerosity;
                numerositySum += cl->numerosity;
            }

            assert(numerositySum > 0);

            if (timeStamp - timeStampNumerositySum / numerositySum > m_constants.thetaGA)
            {
                for (auto && cl : m_set)
                {
                    cl->timeStamp = timeStamp;
                }

                m_ga.run(*this, situation, population);
            }
        }

        // UPDATE SET
        virtual void update(double p, Population & population)
        {
            // Calculate numerosity sum used for updating action set size estimate
            uint64_t numerositySum = 0;
            for (auto && cl : m_set)
            {
                numerositySum += cl->numerosity;
            }

            for (auto && cl : m_set)
            {
                ++cl->experience;

                // Update prediction, prediction error
                if (m_constants.useMAM && cl->experience < 1.0 / m_constants.beta)
                {
                    cl->prediction += (p - cl->prediction) / cl->experience;
                    cl->epsilon += (fabs(p - cl->prediction) - cl->epsilon) / cl->experience;
                }
                else
                {
                    cl->prediction += m_constants.beta * (p - cl->prediction);
                    cl->epsilon += m_constants.beta * (fabs(p - cl->prediction) - cl->epsilon);
                }

                // Update action set size estimate
                if (cl->experience < 1.0 / m_constants.beta)
                {
                    cl->actionSetSize += (numerositySum - cl->actionSetSize) / cl->experience;
                }
                else
                {
                    cl->actionSetSize += m_constants.beta * (numerositySum - cl->actionSetSize);
                }
            }

            updateFitness();

            if (m_constants.doActionSetSubsumption)
            {
                doSubsumption(population);
            }
        }
    };

}}
