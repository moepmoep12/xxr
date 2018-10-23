#pragma once

#include <vector>
#include <cstddef>
#include <cassert>
#include <limits>

#include "environment.h"
#include "../XCS/random.h"

namespace XCSR
{

    class CheckerboardEnvironment final : public XCS::AbstractEnvironment<double, bool>
    {
    private:
        const std::size_t m_dim;
        const std::size_t m_division;
        std::vector<double> m_situation;
        bool m_isEndOfProblem;

        static std::vector<double> randomSituation(std::size_t dim)
        {
            std::vector<double> situation;
            for (std::size_t i = 0; i < dim; ++i)
            {
                situation.push_back(XCS::Random::nextDouble());
            }
            return situation;
        }

    public:
        explicit CheckerboardEnvironment(std::size_t dim, std::size_t division) :
            AbstractEnvironment<double, bool>({ false, true }),
            m_dim(dim),
            m_division(division),
            m_situation(randomSituation(dim)),
            m_isEndOfProblem(false)
        {
        }

        ~CheckerboardEnvironment() = default;

        virtual std::vector<double> situation() const override
        {
            return m_situation;
        }

        virtual double executeAction(bool action) override
        {
            double reward = (action == getAnswer()) ? 1000.0 : 0.0;

            // Update situation
            m_situation = randomSituation(m_dim);

            // Single-step problem
            m_isEndOfProblem = true;

            return reward;
        }

        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        // Returns the answer for the current situation
        bool getAnswer() const
        {
            return getAnswer(m_situation);
        }

        // Returns the answer for the given situation
        bool getAnswer(const std::vector<double> & situation) const
        {
            assert(situation.size() == m_dim);

            // Flatten the n-dimensional grid to one dimension
            std::size_t sum = 0;
            static constexpr double EPSILON = std::numeric_limits<double>::epsilon(); // used for restricting the range to [0.0, 1.0)
            for (std::size_t i = 0; i < m_dim; ++i)
            {
                double value = std::min(std::max(situation.at(i), 0.0), 1.0 - EPSILON);
                sum += static_cast<std::size_t>(value * m_division);
            }

            // Alternate black and white colors
            return sum % 2 != 0;
        }
    };

}