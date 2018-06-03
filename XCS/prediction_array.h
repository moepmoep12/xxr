#pragma once

#include <vector>
#include <unordered_map>
#include <random>
#include <limits>
#include <cfloat>
#include <cmath>

#include "classifier.h"
#include "classifier_ptr_set.h"
#include "random.h"

namespace xcs
{

    template <typename T, typename Action, class Symbol = Symbol<T>>
    class AbstractPredictionArray
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier<T, Action>>;

        // PA (Prediction Array)
        std::unordered_map<Action, double> m_pa;

        // Array of PA keys (for random action selection)
        std::vector<Action> m_paActions;

        // The maximum value of PA
        double m_maxPA;

        // The best actions of PA
        std::vector<Action> m_maxPAActions;

    public:
        // GENERATE PREDICTION ARRAY
        explicit AbstractPredictionArray(const MatchSet<T, Action> & matchSet)
        {
            // FSA (Fitness Sum Array)
            std::unordered_map<Action, double> fsa;

            for (auto && cl : matchSet)
            {
                if (m_pa.count(cl->action) == 0) {
                    m_paActions.push_back(cl->action);
                }
                m_pa[cl->action] += cl->prediction * cl->fitness;
                fsa[cl->action] += cl->fitness;
            }

            m_maxPA = std::numeric_limits<double>::lowest();

            for (auto && pair : m_pa)
            {
                if (fabs(fsa[pair.first]) > 0.0)
                {
                    pair.second /= fsa[pair.first];
                }

                // Update the best actions
                if (fabs(m_maxPA - pair.second) < DBL_EPSILON) // m_maxPA == pair.second
                {
                    m_maxPAActions.push_back(pair.first);
                }
                else if (m_maxPA < pair.second)
                {
                    m_maxPAActions.clear();
                    m_maxPAActions.push_back(pair.first);
                    m_maxPA = pair.second;
                }
            }
        }

        virtual ~AbstractPredictionArray() = default;

        virtual double max() const
        {
            assert(m_maxPA == std::numeric_limits<double>::lowest());
            return m_maxPA;
        }

        // SELECT ACTION
        virtual Action selectAction() const = 0;
    };

    template <typename T, typename Action, class Symbol = Symbol<T>>
    class GreedyPredictionArray : public AbstractPredictionArray<T, Action>
    {
    private:
        using AbstractPredictionArray<T, Action>::m_maxPAActions;

    public:
        using AbstractPredictionArray<T, Action>::AbstractPredictionArray;

        Action selectAction() const override
        {
            // Choose best action
            assert(!m_maxPAActions.empty());
            return Random::chooseFrom(m_maxPAActions);
        }
    };

    template <typename T, typename Action, class Symbol = Symbol<T>>
    class EpsilonGreedyPredictionArray : public AbstractPredictionArray<T, Action>
    {
    private:
        double m_epsilon;
        using AbstractPredictionArray<T, Action>::m_paActions;
        using AbstractPredictionArray<T, Action>::m_maxPAActions;

    public:
        EpsilonGreedyPredictionArray(const MatchSet<T, Action> & matchSet, double epsilon)
            : AbstractPredictionArray<T, Action>(matchSet), m_epsilon(epsilon) {}

        Action selectAction() const override
        {
            if (Random::nextDouble() < m_epsilon)
            {
                assert(!m_paActions.empty());
                return Random::chooseFrom(m_paActions); // Choose random action
            }
            else
            {
                assert(!m_maxPAActions.empty());
                return Random::chooseFrom(m_maxPAActions); // Choose best action
            }
        }
    };

}