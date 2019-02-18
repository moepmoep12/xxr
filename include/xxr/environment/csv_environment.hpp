#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cstddef>
#include <cassert>

#include "environment.hpp"
#include "../random.hpp"

namespace xxr
{

    template <typename T, typename Action>
    class CSVEnvironment : public AbstractEnvironment<T, Action>
    {
    protected:
        std::vector<std::vector<T>> m_situations;
        std::vector<Action> m_actions;
        std::vector<T> m_situation;
        Action m_answer;
        std::size_t m_nextIdx;
        const bool m_chooseRandom;
        bool m_isEndOfProblem;

        virtual void loadNext()
        {
            if (m_chooseRandom)
            {
                auto idx = Random::nextInt<std::size_t>(0UL, m_situations.size() - 1UL);
                m_situation = m_situations[idx];
                m_answer = m_actions[idx];
            }
            else
            {
                m_situation = m_situations[m_nextIdx];
                m_answer = m_actions[m_nextIdx];
                if (++m_nextIdx >= m_situations.size())
                {
                    m_nextIdx = 0;
                }
            }
        }

    public:
        CSVEnvironment(const std::string & filename, const std::unordered_set<Action> & availableActions, bool chooseRandom = true) :
            AbstractEnvironment<T, Action>(availableActions),
            m_nextIdx(0),
            m_chooseRandom(chooseRandom),
            m_isEndOfProblem(false)
        {
            // Load all lines from csv
            std::ifstream ifs(filename);
            std::string line;
            while (std::getline(ifs, line) && !line.empty())
            {
                // Split comma-separated string
                std::istringstream iss(line);
                std::string field;
                double fieldValue = NAN;
                std::vector<T> situation;
                while (std::getline(iss, field, ','))
                {
                    fieldValue = std::stof(field);
                    situation.push_back(static_cast<T>(fieldValue));
                }
                assert(!std::isnan(fieldValue));

                // Last field is action
                m_actions.push_back(static_cast<Action>(fieldValue));
                situation.pop_back();

                m_situations.push_back(situation);
            }

            assert(!m_situations.empty());
            assert(!m_actions.empty());
            assert(m_situations.size() == m_actions.size());

            loadNext();
        }

        virtual ~CSVEnvironment() = default;

        virtual std::vector<T> situation() const override
        {
            return m_situation;
        }

        virtual double executeAction(Action action) override
        {
            double reward = (action == m_answer) ? 1000.0 : 0.0;

            // Single-step problem
            m_isEndOfProblem = true;

            loadNext();

            return reward;
        }

        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        // Returns the answer
        virtual Action getAnswer() const
        {
            return m_answer;
        }
    };

}
