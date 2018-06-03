#pragma once

#include "../XCS/xcs.h"
#include "cs_symbol.h"

template <class Environment, typename T, typename Action, class Symbol = CSSymbol<T>>
class XCSR : public XCS<Environment, T, Action, Symbol>
{
public:
};
