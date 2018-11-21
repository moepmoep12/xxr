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

        enum BlockType
        {
            EMPTY = 0,
            TREE,
            FOOD
        };

        static BlockType charToBlock(unsigned char c)
        {
            switch(c)
            {
            case 'T':
                return TREE;
            case 'F':
                return FOOD;
            default:
                return EMPTY;
            }
        }

        static std::vector<bool> blockToBits(BlockType block)
        {
            switch(block)
            {
            case TREE:
                return { 0, 1 }; // Tree (Obstacle)
            case FOOD:
                return { 1, 1 }; // Food
            default:
                return { 0, 0 }; // Empty
            }
        }

        void setRandomEmptyPosition()
        {
            auto randomPosition = Random::chooseFrom(m_emptyPositions);
            m_currentX = randomPosition.first;
            m_currentY = randomPosition.second;
        }

        virtual BlockType getBlock(int x, int y) const
        {
            if (x < 0 || y < 0 || x >= static_cast<int>(m_worldWidth) || y >= static_cast<int>(m_worldHeight))
            {
                return TREE;
            }
            else
            {
                return charToBlock(m_worldMap[y][x]);
            }
        }

    public:
        // Constructor
        explicit BlockWorldEnvironment(const std::string & mapFilename, std::size_t maxStep, bool allowDiagonalAction) :
            AbstractEnvironment<bool, int>(
                allowDiagonalAction
                    ? std::unordered_set<int>{ UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT }
                    : std::unordered_set<int>{ UP, RIGHT, DOWN, LEFT }
            ),
            m_worldWidth(0),
            m_worldHeight(0),
            m_maxStep(maxStep),
            m_currentStep(0),
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
                    if (getBlock(x, y) == EMPTY)
                    {
                        m_emptyPositions.emplace_back(x, y);
                    }
                }
            }

            setRandomEmptyPosition();
        }

        ~BlockWorldEnvironment() = default;

        virtual std::vector<bool> situation() const override
        {
            std::vector<bool> situation;
            for (std::size_t i = 0; i < sizeof(s_xDiffs) / sizeof(int); ++i)
            {
                auto block = getBlock(m_currentX + s_xDiffs[i], m_currentY + s_yDiffs[i]);
                for (auto && bit : blockToBits(block))
                {
                    situation.push_back(bit);
                }
            }
            return situation;
        }

        virtual double executeAction(int action) override
        {
            assert(action >= 0 && action < 8);

            int x = m_currentX + s_xDiffs[action];
            int y = m_currentY + s_yDiffs[action];

            double reward;

            if (getBlock(x, y) == FOOD)
            {
                setRandomEmptyPosition();
                m_isEndOfProblem = true;
                reward = 1000.0;
            }
            else if (getBlock(x, y) == EMPTY)
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

            if (++m_currentStep >= m_maxStep)
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
                    auto block = getBlock(x, y);
                    str +=
                        (block == TREE) ? 'T' :
                        (block == FOOD) ? 'F' :
                        (static_cast<int>(x) == m_currentX && static_cast<int>(y) == m_currentY) ? 'A' : '.';
                }
                str += "\n";
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