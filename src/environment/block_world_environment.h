#pragma once

#include <vector>
#include <unordered_set>
#include <cstddef>
#include <cassert>
#include <fstream>

#include "environment.h"
#include "../XCS/random.h"

namespace XCS
{

    class BlockWorldEnvironment final : public AbstractEnvironment<bool, int>
    {
    private:
        std::size_t m_worldWidth;
        std::size_t m_worldHeight;
        std::vector<std::string> m_worldMap;
        std::vector<std::pair<int, int>> m_emptyPositions;
        int m_currentX;
        int m_currentY;
        std::size_t m_maxStep;
        std::size_t m_currentStep;
        bool m_threeBitMode;
        bool m_isEndOfProblem;

        static constexpr int s_xDiffs[] = {  0, +1, +1, +1,  0, -1, -1, -1 };
        static constexpr int s_yDiffs[] = { -1, -1,  0, +1, +1, +1,  0, -1 };

        enum Direction
        {
            UP = 0,
            UP_RIGHT,
            RIGHT,
            DOWN_RIGHT,
            DOWN,
            DOWN_LEFT,
            LEFT,
            UP_LEFT
        };

        std::vector<bool> charToBits(unsigned char block) const
        {
            if (m_threeBitMode)
            {
                switch(block)
                {
                case 'T':
                case 'O':
                    return { 0, 1, 0 }; // O-obstacle
                case 'Q':
                    return { 0, 1, 1 }; // Q-obstacle
                case 'F':
                    return { 1, 1, 0 }; // Food-F
                case 'G':
                    return { 1, 1, 1 }; // Food-G
                default:
                    return { 0, 0, 0 }; // Empty
                }
            }
            else
            {
                switch(block)
                {
                case 'T':
                case 'O':
                case 'Q':
                    return { 0, 1 }; // Tree (Obstacle)
                case 'F':
                case 'G':
                    return { 1, 1 }; // Food
                default:
                    return { 0, 0 }; // Empty
                }
            }
        }

        void setRandomEmptyPosition()
        {
            auto randomPosition = Random::chooseFrom(m_emptyPositions);
            m_currentX = randomPosition.first;
            m_currentY = randomPosition.second;
        }

    public:
        // Constructor
        BlockWorldEnvironment(const std::string & mapFilename, std::size_t maxStep, bool threeBitMode, bool allowDiagonalAction) :
            AbstractEnvironment<bool, int>(
                allowDiagonalAction
                    ? std::unordered_set<int>{ UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT }
                    : std::unordered_set<int>{ UP, RIGHT, DOWN, LEFT }
            ),
            m_worldWidth(0),
            m_worldHeight(0),
            m_maxStep(maxStep),
            m_currentStep(0),
            m_threeBitMode(threeBitMode),
            m_isEndOfProblem(false)
        {
            std::ifstream ifs(mapFilename);
            std::string line;
            while (std::getline(ifs, line))
            {
                if (m_worldWidth == 0)
                {
                    m_worldWidth = line.length();
                }
                else
                {
                    assert(m_worldWidth == line.length());
                }

                m_worldMap.push_back(line);
                ++m_worldHeight;
            }

            // Store empty positions for random initialization
            for (std::size_t y = 0; y < m_worldHeight; ++y)
            {
                for (std::size_t x = 0; x < m_worldWidth; ++x)
                {
                    if (isEmpty(x, y))
                    {
                        m_emptyPositions.emplace_back(x, y);
                    }
                }
            }

            setRandomEmptyPosition();
        }

        ~BlockWorldEnvironment() = default;

        virtual std::size_t worldWidth() const
        {
            return m_worldWidth;
        }

        virtual std::size_t worldHeight() const
        {
            return m_worldHeight;
        }

        virtual int currentX() const
        {
            return m_currentX;
        }

        virtual int currentY() const
        {
            return m_currentY;
        }

        virtual unsigned char getBlock(int x, int y) const
        {
            while (x < 0)
            {
                x += m_worldWidth;
            }
            while (y < 0)
            {
                y += m_worldHeight;
            }
            return m_worldMap[y % m_worldHeight][x % m_worldWidth];
        }

        bool isEmpty(int x, int y) const
        {
            switch(getBlock(x, y))
            {
            case 'T':
            case 'O':
            case 'Q':
            case 'F':
            case 'G':
                return false;
            default:
                return true;
            }
        }

        bool isFood(int x, int y) const
        {
            switch(getBlock(x, y))
            {
            case 'F':
            case 'G':
                return true;
            default:
                return false;
            }
        }

        bool isObstacle(int x, int y) const
        {
            switch(getBlock(x, y))
            {
            case 'T':
            case 'O':
            case 'Q':
                return true;
            default:
                return false;
            }
        }

        virtual std::vector<bool> situation(int x, int y) const
        {
            std::vector<bool> situation;
            for (std::size_t i = 0; i < sizeof(s_xDiffs) / sizeof(int); ++i)
            {
                auto block = getBlock(x + s_xDiffs[i], y + s_yDiffs[i]);
                for (auto && bit : charToBits(block))
                {
                    situation.push_back(bit);
                }
            }
            return situation;
        }

        virtual std::vector<bool> situation() const override
        {
            return situation(m_currentX, m_currentY);
        }

        virtual double executeAction(int action) override
        {
            assert(action >= 0 && action < 8);

            int x = m_currentX + s_xDiffs[action];
            int y = m_currentY + s_yDiffs[action];

            double reward;

            if (isFood(x, y))
            {
                setRandomEmptyPosition();
                m_isEndOfProblem = true;
                m_currentStep = 0;
                reward = 1000.0;
            }
            else if (isEmpty(x, y))
            {
                m_currentX = x;
                m_currentY = y;
                m_isEndOfProblem = false;
                reward = 0.0;
            }
            else
            {
                m_isEndOfProblem = false;
                reward = 0.0;
            }

            if (!m_isEndOfProblem && ++m_currentStep >= m_maxStep)
            {
                setRandomEmptyPosition();
                m_isEndOfProblem = true;
                m_currentStep = 0;
            }

            return reward;
        }

        virtual bool isEndOfProblem() const override
        {
            return m_isEndOfProblem;
        }

        virtual std::string toString() const
        {
            std::string str;
            for (std::size_t y = 0; y < m_worldHeight; ++y)
            {
                for (std::size_t x = 0; x < m_worldWidth; ++x)
                {
                    str += (static_cast<int>(x) == m_currentX && static_cast<int>(y) == m_currentY) ? '*' : getBlock(x, y);
                }
                str += '\n';
            }
            return str;
        }

        friend std::ostream & operator<< (std::ostream & os, const BlockWorldEnvironment & obj)
        {
            return os << obj.toString();
        }
    };

    constexpr int BlockWorldEnvironment::s_xDiffs[];
    constexpr int BlockWorldEnvironment::s_yDiffs[];

}