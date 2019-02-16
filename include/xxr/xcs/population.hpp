#pragma once

#include "../random.hpp"

namespace xxr { namespace xcs_impl
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet>
    class Population : public ClassifierPtrSet
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using ClassifierPtrSet::m_set;
        using ClassifierPtrSet::m_constants;
        using ClassifierPtrSet::m_availableActions;

        // DELETION VOTE
        virtual double deletionVote(const Classifier & cl, double averageFitness) const
        {
            double vote = cl.actionSetSize * cl.numerosity;

            // Consider fitness for deletion vote
            if ((cl.experience > m_constants.thetaDel) && (cl.fitness / cl.numerosity < m_constants.delta * averageFitness))
            {
                vote *= averageFitness / (cl.fitness / cl.numerosity);
            }

            return vote;
        }

    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet;

        // Destructor
        virtual ~Population() = default;

        // INSERT IN POPULATION
        virtual void insertOrIncrementNumerosity(const ClassifierPtr & cl)
        {
            for (auto && c : m_set)
            {
                if (c->condition == cl->condition && c->action == cl->action)
                {
                    ++c->numerosity;
                    return;
                }
            }
            m_set.insert(cl);
        }

        // DELETE FROM POPULATION
        virtual void deleteExtraClassifiers()
        {
            uint64_t numerositySum = 0;
            double fitnessSum = 0.0;
            for (auto && c : m_set)
            {
                numerositySum += c->numerosity;
                fitnessSum += c->fitness;
            }

            // Return if the sum of numerosity has not met its maximum limit
            if (numerositySum < m_constants.maxPopulationClassifierCount)
            {
                return;
            }

            // The average fitness in the population
            double averageFitness = fitnessSum / numerositySum;

            std::vector<const ClassifierPtr *> targets;
            for (auto && cl : m_set)
            {
                targets.push_back(&cl);
            }

            std::size_t selectedIdx;
            // Note: not using tournament selection in deletion
            /*if (m_constants.tournamentSize > 0.0 && m_constants.tournamentSize <= 1.0)
            {
                // Tournament selection
                std::vector<std::pair<double, std::size_t>> votes;
                votes.reserve(m_set.size());
                for (auto && c : m_set)
                {
                    votes.emplace_back(deletionVote(*c, averageFitness), c->numerosity);
                }
                selectedIdx = Random::tournamentSelection(votes, m_constants.tournamentSize);
            }
            else
            {*/
                // Roulette-wheel selection
                std::vector<double> votes;
                votes.reserve(m_set.size());
                for (auto && c : m_set)
                {
                    votes.push_back(deletionVote(*c, averageFitness));
                }
                selectedIdx = Random::rouletteWheelSelection(votes);
            //}

            // Distrust the selected classifier
            if ((*targets[selectedIdx])->numerosity > 1)
            {
                (*targets[selectedIdx])->numerosity--;
            }
            else
            {
                m_set.erase(*targets[selectedIdx]);
            }
        }
    };

}}
