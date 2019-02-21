#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <cstddef>

namespace xxr { namespace xcs_impl
{

    template <class GA>
    class ActionSet : public GA::ClassifierPtrSetType
    {
    public:
        using type = typename GA::type;
        using SymbolType = typename GA::SymbolType;
        using ConditionType = typename GA::ConditionType;
        using ActionType = typename GA::ActionType;
        using ConstantsType = typename GA::ConstantsType;
        using ClassifierType = typename GA::ClassifierType;
        using StoredClassifierType = typename GA::StoredClassifierType;
        using ClassifierPtrSetType = typename GA::ClassifierPtrSetType;
        using PopulationType = typename GA::PopulationType;
        using GAType = GA;
        using typename GA::ClassifierPtrSetType::ClassifierPtr;

    protected:
        using ClassifierPtrSetType::m_set;
        using ClassifierPtrSetType::m_constants;
        using ClassifierPtrSetType::m_availableActions;

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
        virtual void doSubsumption(PopulationType & population)
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
        ActionSet(ConstantsType & constants, const std::unordered_set<ActionType> & availableActions) :
            ClassifierPtrSetType(constants, availableActions),
            m_ga(constants, availableActions)
        {
        }

        template <class MatchSet>
        ActionSet(const MatchSet & matchSet, ActionType action, ConstantsType & constants, const std::unordered_set<ActionType> & availableActions) :
            ClassifierPtrSetType(constants, availableActions),
            m_ga(constants, availableActions)
        {
            regenerate(matchSet, action);
        }

        // Destructor
        virtual ~ActionSet() = default;

        // GENERATE ACTION SET
        template <class MatchSet>
        void regenerate(const MatchSet & matchSet, ActionType action)
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

        virtual void copyTo(ActionSet<GA> & dest)
        {
            dest.m_set = m_set; // don't copy m_ga since it contains const parameters
        }

        // RUN GA (refer to GA::run() for the latter part)
        virtual void runGA(const std::vector<type> & situation, PopulationType & population, uint64_t timeStamp)
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
        virtual void update(double p, PopulationType & population)
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
