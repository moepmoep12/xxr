#pragma once

#include "../XCS/condition.h"

namespace XCSR
{

    template <typename T, class Symbol>
    class Condition : public XCS::Condition<T, Symbol>
    {
    public
        void randomGeneralize() override = delete;
        std::size_t dontCareCount() const = delete;
    };
}
