#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cstddef>
#include <cassert>

#include "environment.h"
#include "../XCS/random.h"

namespace XCS
{

    template <typename T, typename Action>
    class CSVEnvironment final : public XCS::AbstractEnvironment<T, Action>
    {
    private:
        bool m_chooseRandom;
        std::vector<std::pair<std::vector<T>, Action> > m_dataSet;
        std::vector<T> m_situation;
        Action m_answer;
        std::size_t m_nextIdx;
        bool m_isEndOfProblem;

        void loadNext()
        {
            if (m_chooseRandom)
            {
                auto sample = Random::chooseFrom(m_dataSet);
                m_situation = sample.first;
                m_answer = sample.second;
            }
            else
            {
                m_situation = m_dataSet[m_nextIdx].first;
                m_answer = m_dataSet[m_nextIdx].second;
                if (++m_nextIdx >= m_dataSet.size())
                {
                    m_nextIdx = 0;
                }
            }
        }

    public:
        CSVEnvironment(std::string filename, const std::unordered_set<Action> & availableActions, bool chooseRandom = true) :
            AbstractEnvironment<T, Action>(availableActions),
            m_nextIdx(0),
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
                Action answer = static_cast<Action>(fieldValue);
                situation.pop_back();

                m_dataSet.emplace_back(situation, answer);
            }

            assert(!m_dataSet.empty());

            loadNext();
        }

        ~CSVEnvironment() = default;

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
        Action getAnswer() const
        {
            return m_answer;
        }
    };

}