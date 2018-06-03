#pragma once

#include <unordered_set>
#include <unordered_map>
#include <cstdint>

#include "classifier_ptr_set.h"
#include "match_set.h"
#include "ga.h"

template <class Symbol, typename Action>
class ActionSet : public ClassifierPtrSet<Symbol, Action>
{
protected:
    using ClassifierPtr = std::shared_ptr<Classifier<Symbol, Action>>;
    using ClassifierPtrSet<Symbol, Action>::m_set;
    using ClassifierPtrSet<Symbol, Action>::m_constants;
    using ClassifierPtrSet<Symbol, Action>::m_actionChoices;

    GA<Symbol, Action> m_ga;

    // UPDATE FITNESS
    void updateFitness()
    {
        double accuracySum = 0.0;

        // Accuracy vector
        std::unordered_map<const ClassifierPtr *, double> kappa(m_set.size());

        for (auto && cl : m_set)
        {
            if (cl->predictionError < m_constants.predictionErrorThreshold)
            {
                kappa[&cl] = 1.0;
            }
            else
            {
                kappa[&cl] = m_constants.alpha * pow(cl->predictionError / m_constants.predictionErrorThreshold, -m_constants.nu);
            }

            accuracySum += kappa[&cl] * cl->numerosity;
        }

        for (auto && cl : m_set)
        {
            cl->fitness += m_constants.learningRate * (kappa[&cl] * cl->numerosity / accuracySum - cl->fitness);
        }
    }

    // DO ACTION SET SUBSUMPTION
    void doSubsumption(Population<Symbol, Action> & population)
    {
        ClassifierPtr cl;

        for (auto && c : m_set)
        {
            if (c->isSubsumer())
            {
                size_t cDontCareCount;
                size_t clDontCareCount;
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
    ActionSet(const XCSConstants & constants, const std::unordered_set<Action> & actionChoices) :
        ClassifierPtrSet<Symbol, Action>(constants, actionChoices),
        m_ga(constants.crossoverProbability, constants.mutationProbability, constants.doGASubsumption, actionChoices)
    {
    }

    ActionSet(const MatchSet<Symbol, Action> & matchSet, Action action, const XCSConstants & constants, const std::unordered_set<Action> & actionChoices) :
        ClassifierPtrSet<Symbol, Action>(constants, actionChoices),
        m_ga(constants.crossoverProbability, constants.mutationProbability, constants.doGASubsumption, actionChoices)
    {
        regenerate(matchSet, action);
    }

    // GENERATE ACTION SET
    void regenerate(const MatchSet<Symbol, Action> & matchSet, Action action)
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

    void copyTo(ActionSet<Symbol, Action> & dest)
    {
        dest.m_set = m_set; // don't copy m_ga since it contains const parameters
    }

    // RUN GA (refer to GA::run() for the latter part)
    void runGA(const Condition<Symbol> & situation, Population<Symbol, Action> & population, uint64_t timeStamp)
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
    void update(double p, Population<Symbol, Action> & population)
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

            // Update prediction, prediction error, and action set size estimate
            if (cl->experience < 1.0 / m_constants.learningRate)
            {
                cl->prediction += (p - cl->prediction) / cl->experience;
                cl->predictionError += (fabs(p - cl->prediction) - cl->predictionError) / cl->experience;
                cl->actionSetSize += (numerositySum - cl->actionSetSize) / cl->experience;
            }
            else
            {
                cl->prediction += m_constants.learningRate * (p - cl->prediction);
                cl->predictionError += m_constants.learningRate * (fabs(p - cl->prediction) - cl->predictionError);
                cl->actionSetSize += m_constants.learningRate * (numerositySum - cl->actionSetSize);
            }
        }

        updateFitness();

        if (m_constants.doActionSetSubsumption)
        {
            doSubsumption(population);
        }
    }
};