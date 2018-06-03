#pragma once

#include "../XCS/experiment.h"
#include "symbol.h"

namespace xcsr
{

    template <typename T, typename Action, class Symbol = xcsr::Symbol<T>>
    class Experiment : public xcs::Experiment<T, Action, Symbol>
    {
    public:
    };

}