#include <iostream>
#include <memory>
#include <string>

#include "situation.h"
#include "classifier.h"
#include "xcs.h"

int main()
{
    XCS<MultiplexerEnvironment, BinarySymbol, bool> xcs(MultiplexerEnvironment(11), XCSConstants());
    xcs.run(1000000);
    xcs.dumpPopulation();

    return 0;
}