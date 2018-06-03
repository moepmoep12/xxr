#include <iostream>
#include <memory>
#include <string>

#include "condition.h"
#include "classifier.h"
#include "xcs.h"

int main()
{
    size_t multiplexerLength = 6;

    XCSConstants constants;

    if (multiplexerLength == 3)
    {
        constants.maxPopulationClassifierCount = 200;
    }
    else if (multiplexerLength == 6)
    {
        constants.maxPopulationClassifierCount = 400;
    }
    else if (multiplexerLength == 11)
    {
        constants.maxPopulationClassifierCount = 800;
    }
    else if (multiplexerLength == 20)
    {
        constants.maxPopulationClassifierCount = 2000;
        constants.generalizeProbability = 0.5;
    }
    else if (multiplexerLength == 37)
    {
        constants.maxPopulationClassifierCount = 5000;
        constants.generalizeProbability = 0.65;
    }
    else
    {
        constants.maxPopulationClassifierCount = 50000;
        constants.generalizeProbability = 0.75;
    }

    XCS<MultiplexerEnvironment, Symbol<bool>, bool> xcs(MultiplexerEnvironment(multiplexerLength), constants);
    xcs.run(1000000);
    xcs.dumpPopulation();

    return 0;
}