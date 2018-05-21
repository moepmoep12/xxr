#pragma once

#include "symbol.h"
#include "classifier_repository.h"

template <class S = BinarySymbol, class Action = std::string>
class ActionSet : public ClassifierRepository<S, Action>
{
public:
};