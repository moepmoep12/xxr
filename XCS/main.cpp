#include <iostream>
#include <string>
#include "state.h"

int main()
{
    std::vector<std::pair<std::string, std::string>> statePairs{
        { "#11##0#", "#11##0#" },
        { "#11##0#", "#11#00#" },
        { "#11##0#", "0111001" },
        { "0111001", "#1##00#" },
        { "0111001", "0111001" },
    };

    for (auto && statePair : statePairs)
    {
        std::cout << statePair.first << std::endl << statePair.second << ": " << (State<>(statePair.first).contains(State<>(statePair.second))) << std::endl << std::endl;
    }

    getchar();

    return 0;
}