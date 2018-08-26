#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cstddef>
#include <cassert>

#include "random.h"
#include "symbol.h"

namespace XCS
{

    template <typename T, typename Action, class Symbol>
    class AbstractEnvironment
    {
    protected:
        // Constructor with available action choices
        AbstractEnvironment(const std::unordered_set<Action> & availableActions) : availableActions(availableActions)
        {
        }

    public:
        // Available action choices
        const std::unordered_set<Action> availableActions;

        // Disable default constructor to force derived classes to set available action choices
        AbstractEnvironment() = delete;

        // Destructor
        virtual ~AbstractEnvironment() = default;

        // Returns current situation
        virtual std::vector<T> situation() const = 0;

        // Executes action (and update situation) and returns reward
        virtual double executeAction(Action action) = 0;

        // Returns true if the problem was solved by the previous action
        // (always true for a single-step problem after the first action execution)
        virtual bool isEndOfProblem() const = 0;
    };

    class MultiplexerEnvironment final : public AbstractEnvironment<bool, bool, Symbol<bool>>
    {
    private:
        const std::size_t m_totalLength;
        const std::size_t m_addressBitLength;
        const std::size_t m_registerBitLength;
        std::vector<bool> m_situation;
        bool m_isEndOfProblem;

        // Get address bit length from total length
        static constexpr std::size_t addressBitLength(std::size_t l, std::size_t c)
        {
            return (l == 0) ? c - 1 : addressBitLength(l >> 1, c + 1);
        }

        static std::vector<bool> randomSituation(std::size_t totalLength)
        {
            std::vector<bool> situation;
            for (std::size_t i = 0; i < totalLength; ++i)
            {
                situation.push_back(Random::nextInt(0, 1));
            }
            return situation;
        }

    public:
        // Constructor
        explicit MultiplexerEnvironment(std::size_t length) :
            AbstractEnvironment<bool, bool, Symbol<bool>>({ false, true }),
            m_totalLength(length),
            m_addressBitLength(addressBitLength(length, 0)),
            m_registerBitLength(length - m_addressBitLength),
            m_situation(randomSituation(length)),
            m_isEndOfProblem(false)
        {
            // Total length must be n + 2^n (n > 0)
            assert(m_totalLength == (m_addressBitLength + ((std::size_t)1 << m_addressBitLength)));
        }

        ~MultiplexerEnvironment() = default;

        std::vector<bool> situation() const override
        {
            return m_situation;
        }

        double executeAction(bool action) override
        {
            double reward = (action == getAnswer(m_situation)) ? 1000.0 : 0.0;

            // Update situation
            m_situation = randomSituation(m_totalLength);

            // Single-step problem
            m_isEndOfProblem = true;

            return reward;
        }

        bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        // Returns answer to situation
        bool getAnswer(const std::vector<bool> & situation) const
        {
            std::size_t address = 0;
            for (std::size_t i = 0; i < m_addressBitLength; ++i)
            {
                if (situation.at(i) == true)
                {
                    address += (std::size_t)1 << (m_addressBitLength - i - 1);
                }
            }

            return situation.at(m_addressBitLength + address) == true;
        }
    };

    template <typename T, typename Action, class Symbol>
    class CSVEnvironment final : public XCS::AbstractEnvironment<T, Action, Symbol>
    {
    private:
        std::ifstream m_ifs;
        std::vector<T> m_situation;
        Action m_answer;
        bool m_isEndOfProblem;

        void loadNext()
        {
            // Get next line
            std::string line;
            if (std::getline(m_ifs, line) && !line.empty())
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
                m_answer = static_cast<Action>(fieldValue);
                situation.pop_back();

                m_situation = situation;
            }
            else
            {
                m_situation = std::vector<T>();
            }
        }

    public:
        explicit CSVEnvironment(std::string filename, const std::unordered_set<Action> & availableActions) :
            AbstractEnvironment<T, Action, Symbol>(availableActions),
            m_ifs(filename),
            m_isEndOfProblem(false)
        {
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
        virtual Action getAnswer(const std::vector<T> & situation) const
        {
            return m_answer;
        }
    };

}