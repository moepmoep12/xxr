#pragma once

#include "../XCS/xcs.h"
#include "cs_symbol.h"

template <typename T, typename Action, class Symbol = CSSymbol<T>>
class XCSR : public XCS<T, Action, Symbol>
{
public:
};
