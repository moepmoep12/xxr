#pragma once

#include "symbol.h"
#include "classifier_repository.h"

template <class S = BinarySymbol, class Action = std::string>
class MatchSet : public ClassifierRepository<S, Action>
{
public:
};