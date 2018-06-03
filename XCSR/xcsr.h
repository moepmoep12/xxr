#pragma once

#include "../XCS/xcs.h"
#include "cs_symbol.h"

namespace xcsr
{

    template <typename T, typename Action, class Symbol = CSSymbol<T>>
    class XCSR : public xcs::XCS<T, Action, Symbol>
    {
    public:
    };

}