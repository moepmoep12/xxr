#include <iostream>
#include <memory>
#include <string>
#include <cstddef>

#include "experiment.h"

using namespace XCSR;

int main()
{
    std::size_t multiplexerLength = 6;

    Constants constants;

    constants.thetaGA = 12;

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
    }
    else if (multiplexerLength == 37)
    {
        constants.maxPopulationClassifierCount = 5000;
    }
    else
    {
        constants.maxPopulationClassifierCount = 50000;
    }

    Experiment<double, bool> xcsr(std::make_shared<RealMultiplexerEnvironment>(multiplexerLength, false), constants);
    xcsr.run(1000000);
    xcsr.dumpPopulation(0.001);

    return 0;
}