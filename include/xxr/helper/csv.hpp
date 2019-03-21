#pragma once
#include <istream>
#include <sstream>
#include <vector>
#include <cmath>

#include "dataset.hpp"

namespace xxr
{

    namespace CSV
    {
        template <typename T, typename Action>
        Dataset<T, Action> readDataset(std::istream & is, bool rounds = false)
        {
            std::vector<std::vector<T>> situations;
            std::vector<Action> actions;

            // Load all lines from csv
            std::string line;
            while (std::getline(is, line) && !line.empty())
            {
                // Split comma-separated string
                std::istringstream iss(line);
                std::string field;
                double fieldValue;
                std::vector<T> situation;
                bool empty = true;
                while (std::getline(iss, field, ','))
                {
                    fieldValue = std::stof(field);
                    situation.push_back(static_cast<T>(rounds ? std::round(fieldValue) : fieldValue));
                    empty = false;
                }

                if (empty)
                {
                    break; // this gets rid of maybe-uninitialized warning
                }

                // Last field is action
                actions.push_back(static_cast<Action>(fieldValue));
                situation.pop_back();

                situations.push_back(situation);
            }

            return { situations, actions };
        }

        template <typename T, typename Action>
        Dataset<T, Action> readDataset(const std::string & filename, bool rounds = false)
        {
            std::ifstream ifs(filename);
            return readDataset<T, Action>(ifs, rounds);
        }
    }

}
